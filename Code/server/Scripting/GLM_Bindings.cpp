
namespace Script
{
namespace
{

void BindVec3(sol::state_view aState)
{
    auto vec3Type = aState.new_usertype<glm::vec3>(
        "vec3", sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>());

    vec3Type["x"] = &glm::vec3::x;
    vec3Type["y"] = &glm::vec3::y;
    vec3Type["z"] = &glm::vec3::z;

    // Bind the common operations
    vec3Type[sol::meta_function::addition] =
        sol::overload(static_cast<glm::vec3 (*)(const glm::vec3&, const glm::vec3&)>(glm::operator+),
                      static_cast<glm::vec3 (*)(const glm::vec3&, float)>(glm::operator+),
                      static_cast<glm::vec3 (*)(float, const glm::vec3&)>(glm::operator+));
    vec3Type[sol::meta_function::subtraction] =
        sol::overload(static_cast<glm::vec3 (*)(const glm::vec3&, const glm::vec3&)>(glm::operator-),
                      static_cast<glm::vec3 (*)(const glm::vec3&, float)>(glm::operator-),
                      static_cast<glm::vec3 (*)(float, const glm::vec3&)>(glm::operator-));
    vec3Type[sol::meta_function::multiplication] =
        sol::overload(static_cast<glm::vec3 (*)(const glm::vec3&, const glm::vec3&)>(glm::operator*),
                      static_cast<glm::vec3 (*)(const glm::vec3&, float)>(glm::operator*),
                      static_cast<glm::vec3 (*)(float, const glm::vec3&)>(glm::operator*));
    vec3Type[sol::meta_function::division] =
        sol::overload(static_cast<glm::vec3 (*)(const glm::vec3&, const glm::vec3&)>(glm::operator/),
                      static_cast<glm::vec3 (*)(const glm::vec3&, float)>(glm::operator/),
                      static_cast<glm::vec3 (*)(float, const glm::vec3&)>(glm::operator/));
    vec3Type[sol::meta_function::equal_to] = [](const glm::vec3& a, const glm::vec3& b) -> bool { 
        return a == b; };
}

void BindVec2(sol::state_view aState)
{
    auto vec2Type = aState.new_usertype<glm::vec2>(
        "vec2", sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>());
    vec2Type["x"] = &glm::vec2::x;
    vec2Type["y"] = &glm::vec2::y;

    // Bind the common operations
    vec2Type[sol::meta_function::addition] =
        sol::overload(static_cast<glm::vec2 (*)(const glm::vec2&, const glm::vec2&)>(glm::operator+),
                      static_cast<glm::vec2 (*)(const glm::vec2&, float)>(glm::operator+),
                      static_cast<glm::vec2 (*)(float, const glm::vec2&)>(glm::operator+));
    vec2Type[sol::meta_function::subtraction] =
        sol::overload(static_cast<glm::vec2 (*)(const glm::vec2&, const glm::vec2&)>(glm::operator-),
                      static_cast<glm::vec2 (*)(const glm::vec2&, float)>(glm::operator-),
                      static_cast<glm::vec2 (*)(float, const glm::vec2&)>(glm::operator-));
    vec2Type[sol::meta_function::multiplication] =
        sol::overload(static_cast<glm::vec2 (*)(const glm::vec2&, const glm::vec2&)>(glm::operator*),
                      static_cast<glm::vec2 (*)(const glm::vec2&, float)>(glm::operator*),
                      static_cast<glm::vec2 (*)(float, const glm::vec2&)>(glm::operator*));
    vec2Type[sol::meta_function::division] =
        sol::overload(static_cast<glm::vec2 (*)(const glm::vec2&, const glm::vec2&)>(glm::operator/),
                      static_cast<glm::vec2 (*)(const glm::vec2&, float)>(glm::operator/),
                      static_cast<glm::vec2 (*)(float, const glm::vec2&)>(glm::operator/));
}

#include <glm/vec4.hpp>

// TBD
void BindVec4(sol::state_view aState)
{
    auto vec4Type = aState.new_usertype<glm::vec4>(
        "vec4", sol::constructors<glm::vec4(), glm::vec4(float), glm::vec4(float, float, float, float)>());
    vec4Type["x"] = &glm::vec4::x;
    vec4Type["y"] = &glm::vec4::y;
    vec4Type["z"] = &glm::vec4::z;
    vec4Type["w"] = &glm::vec4::w;

#if 0
      // Bind the common operations
    vec4Type[sol::meta_function::addition] =
        sol::overload(static_cast<glm::vec4 (*)(const glm::vec4&, const glm::vec4&)>(glm::operator+),
                      static_cast<glm::vec4 (*)(const glm::vec4&, float)>(glm::operator+),
                      static_cast<glm::vec4 (*)(float, const glm::vec4&)>(glm::operator+));
    vec4Type[sol::meta_function::subtraction] =
        sol::overload(static_cast<glm::vec4 (*)(const glm::vec4&, const glm::vec4&)>(glm::operator-),
                      static_cast<glm::vec4 (*)(const glm::vec4&, float)>(glm::operator-),
                      static_cast<glm::vec4 (*)(float, const glm::vec4&)>(glm::operator-));
    vec4Type[sol::meta_function::multiplication] =
        sol::overload(static_cast<glm::vec4 (*)(const glm::vec4&, const glm::vec4&)>(glm::operator*),
                      static_cast<glm::vec4 (*)(const glm::vec4&, float)>(glm::operator*),
                      static_cast<glm::vec4 (*)(float, const glm::vec4&)>(glm::operator*));
    vec4Type[sol::meta_function::division] =
        sol::overload(static_cast<glm::vec4 (*)(const glm::vec4&, const glm::vec4&)>(glm::operator/),
                      static_cast<glm::vec4 (*)(const glm::vec4&, float)>(glm::operator/),
                      static_cast<glm::vec4 (*)(float, const glm::vec4&)>(glm::operator/));
#endif
}

void BindMat3(sol::state_view aState)
{
    auto mat3Type = aState.new_usertype<glm::mat3>(
        "mat3", sol::constructors<glm::mat3(), glm::mat3(float), glm::mat3(const glm::mat4&),
                                  glm::mat3(float, float, float, float, float, float, float, float, float)>());

    // Bind the member variables
    mat3Type["[1]"] = sol::property([](const glm::mat3& mat) { return mat[0]; },
                                    [](glm::mat3& mat, const glm::vec3& value) { mat[0] = value; });
    mat3Type["[2]"] = sol::property([](const glm::mat3& mat) { return mat[1]; },
                                    [](glm::mat3& mat, const glm::vec3& value) { mat[1] = value; });
    mat3Type["[3]"] = sol::property([](const glm::mat3& mat) { return mat[2]; },
                                    [](glm::mat3& mat, const glm::vec3& value) { mat[2] = value; });

    // Bind the common operations
    mat3Type[sol::meta_function::addition] =
        sol::overload(static_cast<glm::mat3 (*)(const glm::mat3&, const glm::mat3&)>(glm::operator+),
                      static_cast<glm::mat3 (*)(const glm::mat3&, float)>(glm::operator+),
                      static_cast<glm::mat3 (*)(float, const glm::mat3&)>(glm::operator+));
    mat3Type[sol::meta_function::subtraction] =
        sol::overload(static_cast<glm::mat3 (*)(const glm::mat3&, const glm::mat3&)>(glm::operator-),
                      static_cast<glm::mat3 (*)(const glm::mat3&, float)>(glm::operator-),
                      static_cast<glm::mat3 (*)(float, const glm::mat3&)>(glm::operator-));
    mat3Type[sol::meta_function::multiplication] =
        sol::overload(static_cast<glm::mat3 (*)(const glm::mat3&, const glm::mat3&)>(glm::operator*),
                      // static_cast<glm::mat3 (*)(const glm::mat3&, const glm::vec3&)>(glm::operator*),
                      static_cast<glm::mat3 (*)(const glm::mat3&, float)>(glm::operator*),
                      static_cast<glm::vec3 (*)(const glm::mat3&, const glm::vec3&)>(glm::operator*));
    mat3Type[sol::meta_function::division] =
        sol::overload(static_cast<glm::mat3 (*)(const glm::mat3&, const glm::mat3&)>(glm::operator/),
                      static_cast<glm::mat3 (*)(const glm::mat3&, float)>(glm::operator/),
                      static_cast<glm::mat3 (*)(float, const glm::mat3&)>(glm::operator/));
}

void BindCoreMath(sol::state_view aState)
{
    // Define type aliases for convenience
    using vec2 = glm::vec2;
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;
    using mat3 = glm::mat3;
    using mat4 = glm::mat4;
    sol::table glm = aState.create_named_table("glm");

    // Bind common constants
    glm["pi"] = glm::pi<float>();
    glm["half_pi"] = glm::half_pi<float>();
    glm["quarter_pi"] = glm::quarter_pi<float>();
    glm["two_pi"] = glm::two_pi<float>();

    // Bind common math functions
    glm["sqrt"] = static_cast<float (*)(float)>(glm::sqrt);
    glm["pow"] = static_cast<float (*)(float, float)>(glm::pow);
    glm["exp"] = static_cast<float (*)(float)>(glm::exp);
    glm["log"] = static_cast<float (*)(float)>(glm::log);
    glm["abs"] = static_cast<float (*)(float)>(glm::abs);
    glm["sign"] = static_cast<float (*)(float)>(glm::sign);
    glm["floor"] = static_cast<float (*)(float)>(glm::floor);
    glm["ceil"] = static_cast<float (*)(float)>(glm::ceil);

    glm["round"] = static_cast<float (*)(float)>(glm::round);
    glm["min"] = static_cast<float (*)(float, float)>(glm::min);
    glm["max"] = static_cast<float (*)(float, float)>(glm::max);
    glm["clamp"] = static_cast<float (*)(float, float, float)>(glm::clamp);

    // Bind vector functions
    glm["dot"] = static_cast<float (*)(const vec2&, const vec2&)>(glm::dot);
    glm["dot"] = static_cast<float (*)(const vec3&, const vec3&)>(glm::dot);
    glm["dot"] = static_cast<float (*)(const vec4&, const vec4&)>(glm::dot);
    glm["cross"] = static_cast<vec3 (*)(const vec3&, const vec3&)>(glm::cross);
    glm["normalize"] = static_cast<vec2 (*)(const vec2&)>(glm::normalize);
    glm["normalize"] = static_cast<vec3 (*)(const vec3&)>(glm::normalize);
    glm["normalize"] = static_cast<vec4 (*)(const vec4&)>(glm::normalize);

    // Bind matrix functions
    glm["transpose"] = static_cast<mat3 (*)(const mat3&)>(glm::transpose);
    glm["transpose"] = static_cast<mat4 (*)(const mat4&)>(glm::transpose);
    glm["inverse"] = static_cast<mat3 (*)(const mat3&)>(glm::inverse);
    glm["inverse"] = static_cast<mat4 (*)(const mat4&)>(glm::inverse);
}
} // namespace

void CreateMathBindings(sol::state_view aState)
{
    BindVec2(aState);
    BindVec3(aState);
    BindVec4(aState);
    BindMat3(aState);
    BindCoreMath(aState);
}
} // namespace Script
