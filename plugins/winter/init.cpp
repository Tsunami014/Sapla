#include "plugins/link.hpp"
#include "core.hpp"
#include "base/svgRend.hpp"

void styl() {
    pallete["light"] = "#93C3D7";
    pallete["dark"] = "#2C3A47";
    pallete["shadow"] = "#506873";
    pallete["faded"] = "#7F9AA6";
    pallete["alight"] = "#CFAEA0";
    pallete["adark"] = "#5B3F32";
}

auto r = RegistryBuilder()
    .addStyl(styl);

REGISTER_PLUG(r.build("A wintery theme for the app!"), 12, VERSION)
