#include "../register.hpp"

void styl() {
    pallete = {
        {"light",  "#93C3D7"},
        {"dark",   "#2C3A47"},
        {"shadow", "#506873"},
        {"faded",  "#7F9AA6"},
        {"alight", "#CFAEA0"},
        {"adark",  "#5B3F32"}
    };
}

auto r = RegistryBuilder()
    .addStyl(styl);

REGISTER_PLUG(r.build("A wintery theme for the app!"), 9, 9)
