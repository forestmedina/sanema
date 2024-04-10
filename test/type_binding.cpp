//
// Created by fores on 4/7/2024.
//
//
// Created by fores on 11/12/2023.
//
#include <catch2/catch_all.hpp>
#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <sstream>
#include <test/helpers.h>

struct vec3{
  float x;
  float y;
  float z;
};

TEST_CASE("return external type",
          "[binding]") {
  std::string code(R"--(
function new_vec3 vec3
begin
   var  result vec3;
   set result.x 1.2;
   set result.y 2.4;
   set result.z 3.3;
   return result;
end
)--");
  sanema::SanemaScriptSystem sanema_script_system{1,10};
    sanema_script_system.add_pod_type<vec3>("vec3")->with_field<float>("x", offsetof( vec3, x ))->with_field<float>("y",offsetof( vec3, y))->with_field<float>("z",offsetof( vec3, z ));
  auto script_id=sanema_script_system.add_script(code);
  auto function_id=sanema_script_system.get_function_id<vec3>(script_id,"new_vec3");
  REQUIRE(function_id.has_value());
  if(function_id.has_value()) {
    auto result=sanema_script_system.run_function<vec3>(script_id,function_id.value(),0);
    std::cout<<"result:"<<result.x<<";"<<result.y<<";"<<result.z<<"\n";
    REQUIRE(result.x==1.2f);
    REQUIRE(result.y==2.4f);
    REQUIRE(result.z==3.3f);
  }




}