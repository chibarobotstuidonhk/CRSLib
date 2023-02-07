#pragma once

#include <string_view>
#include <syncstream>
#include <iostream>
#include <vector>
#include <memory>

#include <boost/asio.hpp>

#include <CRSLib/include/std_int.hpp>
#include <CRSLib/include/error_reporter.hpp>

#include "pack.hpp"

namespace CRSLib::Usb
{
	namespace Implement
	{
		struct DefaultReporter final
		{
			void operator()(auto&& ... args) const noexcept
			{
				try
				{
					std::osyncstream sync_err{std::cerr};
					(sync_err << ... << std::forward<decltype(args)>(args)) << std::endl;
				}
				catch(...)
				{
					try
					{
						std::osyncstream sync_err{std::cerr};
						sync_err << "Fail to report error." << std::endl;
					}
					catch(...){}
				}
			}
		};
	}

	// COBS(Consistent Overhead Byte Stuffing)でシリアル通信を行う
	// read, write操作を1つずつ並列に実行できる。どちらか一方でも複数同時に行うことはできない
	// 送受信ともに失敗したら再度試みる。
	// **全く以てシグナル安全でない**
	template<ErrorReporterC ErrorReporter = Implement::DefaultReporter>
	class CobsSerial final
	{
		// io_contextまわり。おそらく、いくつかのスレッドで並行に実行する処理をまとめたモノ
		boost::asio::io_context io{};
		boost::asio::executor_work_guard work_guard{io.get_executor()};  // read, write操作を要求されてないときにio.run()が止まらないようにする

		// 同時に動く処理は高々2つなので
		boost::asio::thread_pool thread_pool{2};
	
		// serial_port。async操作は並列に実行できないこともあるので注意(ex. asnc_writeは並列に呼び出してはならない)
		boost::asio::serial_port port{io};

		// strand。上記の理由により同時に呼び出せないasync_read, async_writeが同時に実行されないことを保証するためのもの
		boost::asio::io_context::strand write_strand{io};
		boost::asio::io_context::strand read_strand{io};

		// エラーを表示するファンクタ
		ErrorReporter error_reporter;


	public:
		CobsSerial(const std::string_view port_filepath, const unsigned int baud_rate, ErrorReporter&& error_reporter = Implement::DefaultReporter()) noexcept(false):
			error_reporter{std::move(error_reporter)}
		{
			// openする
			port.open(port_filepath);

			// baudrateをセット
			port.set_option(asio::serial_port_base::baud_rate(baud_rate));

			// 処理を別スレッドで走らせる
			boost::asio::post(thread_pool,
				[this]
				{
					try
					{
						io.run();
					}
					catch(...)
					{
						error_reporter(__FILE__, __LINE__, ": error: boost::asio::io_context::run() throws an exception.");
					}
				}
			);
		}

		~CobsSerial()
		{
			boost::system::error_code ec;
			try
			{
				port.cancel(ec);
			}
			catch
			{
				error_reporter(__FILE__, __LINE__, ": error: cannot boost::asio::serial_port::cansel.: ", ec.message());
			}
			try
			{
				port.close(ec);
			}
			catch
			{
				error_reporter(__FILE__, __LINE__, ": error: cannot boost::asio::serial_port::close.: ", ec.message());
			}
		}

		// 逆COBS変換されたフレームを受け取るハンドラを受け取る
		template<class ReadHandler>
		requires requires(ReadHandler read_handler, std::vector<u8> frame)
		{
			{read_handler(frame)} noexcept;
		}
		void async_read(ReadHandler&& read_handler) noexcept
		{
			async_read_inner(std::move(read_handler), std::make_unique<std::vector<u8>>());
		}

		void async_write(std::vector<u8> frame) noexcept
		{
			async_write_inner(std::make_unique<std::vector<u8>>(cobs(frame)));
		}

	private:
		// 例外発生時に再帰するのでinnerを用意
		template<class ReadHandler>
		void async_read_inner(ReadHandler&& read_handler, std::unique<std::vector<u8>>&& cobsed_frame) noexcept
		{
			boost::asio::post(read_strand, [this, read_handler = std::move(read_handler), cobsed_frame = std::move(cobsed_frame)]() mutable
			{
				// 引数の評価順序が未規定であることに注意
				const auto cobsed_frame_p = cobsed_frame_up.get();
				boost::asio::async_read_until(port, boost::asio::buffer(*cobsed_frame_p), '\0',
					[this, read_handler = std::move(read_handler), cobsed_frame = std::move(cobsed_frame)](const boost::system::error_code& ec, const size_t) mutable  // ここで寿命が移る
					{
						if(ec)
						{
							error_reporter(__FILE__, __LINE__, ": error: cannot boost::asio::async_read_until.:", ec.message());
							this->async_read_inner(std::move(read_handler, std::move(cobsed_frame)));  // もう一度読む
						}
						else
						{
							read_handler(decobs(*cobsed_frame_up));  // 0終端してあるのでヨシ
						}
					}
				);
			});
		}

		// 例外発生時に再帰するのでinnerを用意
		void async_write_inner(std::unique_ptr<std::vector<u8>>&& cobsed_frame) noexcept
		{
			boost::asio::post(write_strand, [this, cobsed_frame = std::move(cobsed_frame)]() mutable
			{
				// 読み出し用バッファの寿命をちゃんと考える
				// 引数の評価順序が未規定であることに注意
				const auto cobsed_frame_p = cobsed_frame_up.get();
				boost::asio::async_write(port, boost::asio::buffer(*cobsed_frame_p),
					[this, cobsed_frame = std::move(cobsed_frame)](const boost::system::error_code& ec, const size_t) mutable
					{
						if(ec)
						{
							error_reporter(__FILE__, __LINE__, ": error: cannot boost::asio::async_read_until.:", ec.message());
							this->async_write(std::move(cobsed_frame));  // もう一度書く
						}
					}
				);
			}
			);
		}
	};
}