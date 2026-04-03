#include "hal_st/instantiations/NucleoUi.hpp"
#include "hal_st/instantiations/StmEventInfrastructure.hpp"
#include "services/util/DebugLed.hpp"
#include "services/util/DebouncedButton.hpp"

unsigned int hse_value = 24'000'000;

static main_::NUCLEO ui;

void ButtonPressed() {
    ui.ledRed.Set(true);
}

void ButtonReleased() {

    ui.ledRed.Set(false);
}


int main() {
    HAL_Init();

    static main_::StmEventInfrastructure eventInfrastructure;
    static services::DebugLed debugLed(ui.ledGreen);
    static services::DebouncedButton DebouncedButton(ui.buttonOne, ButtonPressed, ButtonReleased);

    eventInfrastructure.Run();

    __builtin_unreachable();
}
