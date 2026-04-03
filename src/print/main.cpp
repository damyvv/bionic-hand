#include "hal_st/instantiations/NucleoUi.hpp"
#include "hal_st/instantiations/StmEventInfrastructure.hpp"
#include "hal_st/stm32fxxx/DmaStm.hpp"
#include "hal_st/stm32fxxx/UartStmDma.hpp"
#include "infra/timer/Timer.hpp"
#include "services/tracer/GlobalTracer.hpp"
#include "services/tracer/StreamWriterOnSerialCommunication.hpp"
#include "services/tracer/Tracer.hpp"
#include "services/tracer/TracerWithDateTime.hpp"
#include "services/util/DebugLed.hpp"
#include <chrono>

unsigned int hse_value = 24'000'000;

int main()
{
    HAL_Init();

    static main_::StmEventInfrastructure eventInfrastructure;
    static main_::NUCLEO ui;
    static services::DebugLed debugLed(ui.ledGreen);
    static hal::DmaStm dmaStm;


    static hal::GpioPinStm stLinkUartTxPin{ hal::Port::D, 8 };
    static hal::GpioPinStm stLinkUartRxPin{ hal::Port::D, 9 };
    static hal::DmaStm::TransmitStream transmitStream{ dmaStm, hal::DmaChannelId{ 1, 3, 4 } };
    static hal::UartStmDma stLinkUartDma{ transmitStream, 3, stLinkUartTxPin, stLinkUartRxPin };


    static services::StreamWriterOnSerialCommunication::WithStorage<64> streamWriterOnSerialCommunication{ stLinkUartDma };

    static infra::TextOutputStream::WithErrorPolicy textOutputStream{ streamWriterOnSerialCommunication };
    static services::TracerWithDateTime tracerWithDateTime{ textOutputStream };

    services::SetGlobalTracerInstance(tracerWithDateTime);

    static infra::TimerRepeating timerRepeating{ std::chrono::seconds{ 1 }, []
        {
            services::GlobalTracer().Trace() << "Hello World !";
        } };

    eventInfrastructure.Run();
    __builtin_unreachable();
}
