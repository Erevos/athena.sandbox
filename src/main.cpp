#include <iostream>

#ifdef _WIN32
	#include <athena.hpp>
	#include <listener.hpp>
	#include <logManager.hpp>
	#include <luaState.hpp>
#else
	#include <athena/athena.hpp>
	#include <athena/listener.hpp>
	#include <athena/logManager.hpp>
	#include <athena/luaState.hpp>
#endif


bool run = true;


class Logger : public athena::core::Listener
{
	public:

		void on_event( const athena::core::Event& event )
		{
			if ( event.code() == athena::EVENT_EXIT )
				run = false;
			else if ( event.code() != athena::EVENT_LOG_NEW_ENTRY )
			{
				athena::io::LogManager* log = athena::io::LogManager::get();


				if ( log != NULL )
				{
					log->log_message("Message code: %d",event.code());

					if ( event.parameter_count() > 0 )
					{
						const athena::core::Parameter* parameter = event.parameter(0);


						if ( parameter != NULL  &&  parameter->data() != NULL )
						{
							switch ( parameter->type() )
							{
							
								case athena::core::DoubleReal:

									log->log_message(L"Update timer: %f ",(*static_cast<double*>(parameter->data())));
									break;

								case athena::core::Pointer:
								
									log->log_message(*(static_cast<std::string*>(parameter->data())));
									break;

								default:

									break;
							}
						}
					}
				}
			}
		};
};

void StringCleanupFunction( const athena::core::Event& event )
{

	for ( unsigned int i = 0;  i < event.parameter_count();  ++i )
	{
		const athena::core::Parameter* parameter = event.parameter(i);


		if ( parameter != NULL  &&  parameter->data() != NULL )
		{
			switch ( parameter->type() )
			{
				case athena::core::Boolean:

					delete static_cast<bool*>(parameter->data());
					break;

				case athena::core::Character:

					delete static_cast<char*>(parameter->data());
					break;

				case athena::core::UnsignedCharacter:

					delete static_cast<unsigned char*>(parameter->data());
					break;

				case athena::core::WideCharacter:

					delete static_cast<wchar_t*>(parameter->data());
					break;

				case athena::core::ShortInteger:

					delete static_cast<short*>(parameter->data());
					break;

				case athena::core::UnsignedShortInteger:

					delete static_cast<unsigned short*>(parameter->data());
					break;

				case athena::core::Integer:

					delete static_cast<int*>(parameter->data());
					break;

				case athena::core::UnsignedInteger:

					delete static_cast<unsigned int*>(parameter->data());
					break;

				case athena::core::LongInteger:

					delete static_cast<long*>(parameter->data());
					break;

				case athena::core::UnsignedLongInteger:

					delete static_cast<unsigned long*>(parameter->data());
					break;

				case athena::core::Real:

					delete static_cast<float*>(parameter->data());
					break;

				case athena::core::DoubleReal:

					delete static_cast<double*>(parameter->data());
					break;

				case athena::core::Pointer:

					delete static_cast<std::string*>(parameter->data());
					break;

				default:

					break;
			}
		}
	}
}


int main( int argc , char** argv )
{

	#ifdef _WIN32

		#ifdef	_DEBUG
			_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		#endif	/* _DEBUG */

	#endif /* _WIN32 */


	if ( athena::init(athena::AthenaManagers::ALL,argc,argv) )
	{
		if ( athena::startup(athena::AthenaManagers::ALL) )
		{
			athena::io::LogManager* log = athena::io::LogManager::get();
			athena::core::Event new_event(athena::ManagerIDs::FirstAvailableID*0x1000);
			athena::core::Event update_event(athena::ManagerIDs::FirstAvailableID*0x1000 + 1);
			Logger entity;
			//std::string input("");


			log->echo(true);
			log->auto_dump_threshold(1);
			log->auto_dump(true);
			log->auto_purge_threshold(1);
			log->auto_purge(true);
			log->auto_dump_file_open_mode(athena::io::LogFileOpenMode::Truncate);
			new_event.cleanup_function(StringCleanupFunction);
			update_event.cleanup_function(StringCleanupFunction);
			update_event.parameter(0,athena::core::DoubleReal,new (std::nothrow) athena::utility::TimerValueType(0));
			athena::trigger_event(athena::core::Event(athena::EVENT_INPUT_INIT_KEYBOARD));
			athena::trigger_event(athena::core::Event(athena::EVENT_INPUT_INIT_MOUSE));
			athena::trigger_event_periodically(update_event,2000);
			//entity.register_event(new_event.code());
			//entity.register_event(update_event.code());
			entity.register_all_events();

			while ( run )
			{
				athena::operate();
			}

			entity.unregister_event(new_event.code());
			entity.unregister_all_events();
			athena::terminate();
		}
	}

	#ifdef _WIN32

		#ifdef	_DEBUG
			_CrtDumpMemoryLeaks();
		#endif	/* _DEBUG */

	#endif /* _WIN32 */


	return 0;
}
