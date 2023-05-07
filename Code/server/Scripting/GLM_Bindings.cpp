
namespace Script
{
namespace
{

void BindVec3(sol::state_view aState)
{
    auto type = aState.new_usertype<glm::vec3>(
        "vec3", sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>());

    type["x"] = &glm::vec3::x;
    type["y"] = &glm::vec3::y;
    type["z"] = &glm::vec3::z;

    // Bind the common operations
    type[sol::meta_function::addition] =
        sol::overload(static_cast<glm::vec3 (*)(const glm::vec3&, const glm::vec3&)>(glm::operator+),
                      static_cast<glm::vec3 (*)(const glm::vec3&, float)>(glm::operator+),
                      static_cast<glm::vec3 (*)(float, const glm::vec3&)>(glm::operator+));
    type[sol::meta_function::subtraction] =
        sol::overload(static_cast<glm::vec3 (*)(const glm::vec3&, const glm::vec3&)>(glm::operator-),
                      static_cast<glm::vec3 (*)(const glm::vec3&, float)>(glm::operator-),
                      static_cast<glm::vec3 (*)(float, const glm::vec3&)>(glm::operator-));
    type[sol::meta_function::multiplication] =
        sol::overload(static_cast<glm::vec3 (*)(const glm::vec3&, const glm::vec3&)>(glm::operator*),
                      static_cast<glm::vec3 (*)(const glm::vec3&, float)>(glm::operator*),
                      static_cast<glm::vec3 (*)(float, const glm::vec3&)>(glm::operator*));
    type[sol::meta_function::division] =
        sol::overload(static_cast<glm::vec3 (*)(const glm::vec3&, const glm::vec3&)>(glm::operator/),
                      static_cast<glm::vec3 (*)(const glm::vec3&, float)>(glm::operator/),
                      static_cast<glm::vec3 (*)(float, const glm::vec3&)>(glm::operator/));
    type[sol::meta_function::equal_to] = [](const glm::vec3& a, const glm::vec3& b) -> bool { 
        return a == b; };
}

void BindVec2(sol::state_view aState)
{
    auto type = aState.new_usertype<glm::vec2>(
        "vec2", sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>());
    type["x"] = &glm::vec2::x;
    type["y"] = &glm::vec2::y;

    // Bind the common operations
    type[sol::meta_function::addition] =
        sol::overload(static_cast<glm::vec2 (*)(const glm::vec2&, const glm::vec2&)>(glm::operator+),
                      static_cast<glm::vec2 (*)(const glm::vec2&, float)>(glm::operator+),
                      static_cast<glm::vec2 (*)(float, const glm::vec2&)>(glm::operator+));
    type[sol::meta_function::subtraction] =
        sol::overload(static_cast<glm::vec2 (*)(const glm::vec2&, const glm::vec2&)>(glm::operator-),
                      static_cast<glm::vec2 (*)(const glm::vec2&, float)>(glm::operator-),
                      static_cast<glm::vec2 (*)(float, const glm::vec2&)>(glm::operator-));
    type[sol::meta_function::multiplication] =
        sol::overload(static_cast<glm::vec2 (*)(const glm::vec2&, const glm::vec2&)>(glm::operator*),
                      static_cast<glm::vec2 (*)(const glm::vec2&, float)>(glm::operator*),
                      static_cast<glm::vec2 (*)(float, const glm::vec2&)>(glm::operator*));
    type[sol::meta_function::division] =
        sol::overload(static_cast<glm::vec2 (*)(const glm::vec2&, const glm::vec2&)>(glm::operator/),
                      static_cast<glm::vec2 (*)(const glm::vec2&, float)>(glm::operator/),
                      static_cast<glm::vec2 (*)(float, const glm::vec2&)>(glm::operator/));
}

#include <glm/vec4.hpp>

// TBD
void BindVec4(sol::state_view aState)
{
    auto type = aState.new_usertype<glm::vec4>(
        "vec4", sol::constructors<glm::vec4(), glm::vec4(float), glm::vec4(float, float, float, float)>());
    type["x"] = &glm::vec4::x;
    type["y"] = &glm::vec4::y;
    type["z"] = &glm::vec4::z;
    type["w"] = &glm::vec4::w;

#if 0
      // Bind the common operations
    type[sol::meta_function::addition] =
        sol::overload(static_cast<glm::vec4 (*)(const glm::vec4&, const glm::vec4&)>(glm::operator+),
                      static_cast<glm::vec4 (*)(const glm::vec4&, float)>(glm::operator+),
                      static_cast<glm::vec4 (*)(float, const glm::vec4&)>(glm::operator+));
    type[sol::meta_function::subtraction] =
        sol::overload(static_cast<glm::vec4 (*)(const glm::vec4&, const glm::vec4&)>(glm::operator-),
                      static_cast<glm::vec4 (*)(const glm::vec4&, float)>(glm::operator-),
                      static_cast<glm::vec4 (*)(float, const glm::vec4&)>(glm::operator-));
    type[sol::meta_function::multiplication] =
        sol::overload(static_cast<glm::vec4 (*)(const glm::vec4&, const glm::vec4&)>(glm::operator*),
                      static_cast<glm::vec4 (*)(const glm::vec4&, float)>(glm::operator*),
                      static_cast<glm::vec4 (*)(float, const glm::vec4&)>(glm::operator*));
    type[sol::meta_function::division] =
        sol::overload(static_cast<glm::vec4 (*)(const glm::vec4&, const glm::vec4&)>(glm::operator/),
                      static_cast<glm::vec4 (*)(const glm::vec4&, float)>(glm::operator/),
                      static_cast<glm::vec4 (*)(float, const glm::vec4&)>(glm::operator/));
#endif
}

void BindMat3(sol::state_view aState)
{
    auto type = aState.new_usertype<glm::mat3>(
        "mat3", sol::constructors<glm::mat3(), glm::mat3(float), glm::mat3(const glm::mat4&),
                                  glm::mat3(float, float, float, float, float, float, float, float, float)>());

    // Bind the member variables
    type["[1]"] = sol::property([](const glm::mat3& mat) { return mat[0]; },
                                    [](glm::mat3& mat, const glm::vec3& value) { mat[0] = value; });
    type["[2]"] = sol::property([](const glm::mat3& mat) { return mat[1]; },
                                    [](glm::mat3& mat, const glm::vec3& value) { mat[1] = value; });
    type["[3]"] = sol::property([](const glm::mat3& mat) { return mat[2]; },
                                    [](glm::mat3& mat, const glm::vec3& value) { mat[2] = value; });

    // Bind the common operations
    type[sol::meta_function::addition] =
        sol::overload(static_cast<glm::mat3 (*)(const glm::mat3&, const glm::mat3&)>(glm::operator+),
                      static_cast<glm::mat3 (*)(const glm::mat3&, float)>(glm::operator+),
                      static_cast<glm::mat3 (*)(float, const glm::mat3&)>(glm::operator+));
    type[sol::meta_function::subtraction] =
        sol::overload(static_cast<glm::mat3 (*)(const glm::mat3&, const glm::mat3&)>(glm::operator-),
                      static_cast<glm::mat3 (*)(const glm::mat3&, float)>(glm::operator-),
                      static_cast<glm::mat3 (*)(float, const glm::mat3&)>(glm::operator-));
    type[sol::meta_function::multiplication] =
        sol::overload(static_cast<glm::mat3 (*)(const glm::mat3&, const glm::mat3&)>(glm::operator*),
                      // static_cast<glm::mat3 (*)(const glm::mat3&, const glm::vec3&)>(glm::operator*),
                      static_cast<glm::mat3 (*)(const glm::mat3&, float)>(glm::operator*),
                      static_cast<glm::vec3 (*)(const glm::mat3&, const glm::vec3&)>(glm::operator*));
    type[sol::meta_function::division] =
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
