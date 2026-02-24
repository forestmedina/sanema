#include <catch2/catch_all.hpp>
#include "SanemaScriptSystem.h"
#include "SanemaScriptSystemImpl.h"
#include "helpers.h"

TEST_CASE("Simple If", "[Branching]") {
    sanema::SanemaScriptSystem script_system{1, 10};
    auto script_id = script_system.add_script(R"(
        function main int64
        begin
            var a int64;
            set a 10;
            if greater(a 5)
                set a 20;
            end
            return a;
        end
    )");


    auto result = script_system.run_function<std::int64_t>(script_id, "main", 0);
    REQUIRE(result == 20);
}
