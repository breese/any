#include <string>
#include <boost/core/lightweight_test.hpp>
#include <boost/any.hpp>

//-----------------------------------------------------------------------------

namespace api_suite
{

void api_ctor_default()
{
    boost::any value;
    BOOST_TEST(value.holds<void>());
}

void api_ctor_default_const()
{
    const boost::any value;
    BOOST_TEST(value.holds<void>());
}

void api_ctor_copy()
{
    boost::any value = 42;
    BOOST_TEST(value.holds<int>());
    BOOST_TEST_EQ(boost::any_cast<int>(value), 42);

    boost::any copy = value;
    BOOST_TEST(copy.holds<int>());
    BOOST_TEST_EQ(boost::any_cast<int>(copy), 42);
}

void api_ctor_move()
{
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    boost::any value = 42;
    BOOST_TEST(value.holds<int>());
    BOOST_TEST_EQ(boost::any_cast<int>(value), 42);

    boost::any copy = boost::move(value);
    BOOST_TEST(copy.holds<int>());
    BOOST_TEST_EQ(boost::any_cast<int>(copy), 42);
#endif
}

void api_ctor_value_copied()
{
    std::string text = "test message";
    boost::any value = text;
    BOOST_TEST(value.holds<std::string>());
    BOOST_TEST_EQ(boost::any_cast<std::string>(value), text);
}

void api_ctor_value_moved()
{
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    std::string text = "test message";
    boost::any value = boost::move(text);
    BOOST_TEST(value.holds<std::string>());
#endif
}

void api_assign_copy()
{
    boost::any value = 42;
    BOOST_TEST(value.holds<int>());
    BOOST_TEST_EQ(boost::any_cast<int>(value), 42);

    boost::any copy;
    copy = value;
    BOOST_TEST(copy.holds<int>());
    BOOST_TEST_EQ(boost::any_cast<int>(copy), 42);
}

void api_assign_move()
{
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    boost::any value = 42;
    BOOST_TEST(value.holds<int>());
    BOOST_TEST_EQ(boost::any_cast<int>(value), 42);

    boost::any copy;
    copy = boost::move(value);
    BOOST_TEST(copy.holds<int>());
    BOOST_TEST_EQ(boost::any_cast<int>(copy), 42);
#endif
}

void api_assign_value_copied()
{
    std::string text = "test message";
    boost::any value;
    BOOST_TEST(value.holds<void>());

    value = text;
    BOOST_TEST(value.holds<std::string>());
    BOOST_TEST_EQ(boost::any_cast<std::string>(value), text);
}

void api_assign_value_moved()
{
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    std::string text = "test message";
    boost::any value;
    BOOST_TEST(value.holds<void>());

    value = boost::move(text);
    BOOST_TEST(value.holds<std::string>());
    BOOST_TEST_EQ(boost::any_cast<std::string>(value), text);
#endif
}

void api_emplace()
{
    boost::any value;
    BOOST_TEST(value.holds<void>());

    value.emplace<std::string>("test message");
    BOOST_TEST(value.holds<std::string>());
}

void api_emplace_initializer_list()
{
#ifndef BOOST_NO_CXX11_HDR_INITIALIZER_LIST
    boost::any value;
    BOOST_TEST(value.holds<void>());

    value.emplace<std::string>({ 'a', 'b', 'c' });
    BOOST_TEST(value.holds<std::string>());
#endif
}

void api_has_value()
{
    boost::any value;
    BOOST_TEST(!value.has_value());
    value = 42;
    BOOST_TEST(value.has_value());
}

void api_empty()
{
    boost::any value;
    BOOST_TEST(value.empty());
    value = 42;
    BOOST_TEST(!value.empty());
}

void api_holds()
{
    struct unknown {};

    boost::any value;
    BOOST_TEST(value.holds<void>());
    BOOST_TEST(!value.holds<int>());
    BOOST_TEST(!value.holds<std::string>());
    BOOST_TEST(!value.holds<unknown>());

    value = 42;
    BOOST_TEST(!value.holds<void>());
    BOOST_TEST(value.holds<int>());
    BOOST_TEST(!value.holds<std::string>());
    BOOST_TEST(!value.holds<unknown>());

    value = std::string("test message");
    BOOST_TEST(!value.holds<void>());
    BOOST_TEST(!value.holds<int>());
    BOOST_TEST(value.holds<std::string>());
    BOOST_TEST(!value.holds<unknown>());
}

void api_holds_decayed()
{
    boost::any value;
    BOOST_TEST(!value.holds<int>());
    BOOST_TEST(!value.holds<int&>());
    BOOST_TEST(!value.holds<const int&>());
    BOOST_TEST(!value.holds<volatile int&>());
    BOOST_TEST(!value.holds<const volatile int&>());

    value = 42;
    BOOST_TEST(value.holds<int>());
    BOOST_TEST(value.holds<int&>());
    BOOST_TEST(value.holds<const int&>());
    BOOST_TEST(value.holds<volatile int&>());
    BOOST_TEST(value.holds<const volatile int&>());
}

void api_swap()
{
    boost::any alpha;
    boost::any bravo = 42;
    BOOST_TEST(alpha.holds<void>());
    BOOST_TEST(bravo.holds<int>());

    alpha.swap(bravo);
    BOOST_TEST(alpha.holds<int>());
    BOOST_TEST(bravo.holds<void>());
}

void api_reset()
{
    boost::any value = 42;
    BOOST_TEST(value.holds<int>());

    value.reset();
    BOOST_TEST(value.holds<void>());
}

void api_clear()
{
    boost::any value = 42;
    BOOST_TEST(value.holds<int>());

    value.clear();
    BOOST_TEST(value.holds<void>());
}

void run()
{
    api_ctor_default();
    api_ctor_default_const();
    api_ctor_copy();
    api_ctor_move();
    api_ctor_value_copied();
    api_ctor_value_moved();
    api_assign_copy();
    api_assign_move();
    api_assign_value_copied();
    api_emplace();
    api_emplace_initializer_list();
    api_has_value();
    api_empty();
    api_holds();
    api_holds_decayed();
    api_swap();
    api_reset();
    api_clear();
}

} // namespace api_suite

//-----------------------------------------------------------------------------

namespace type_suite
{

void type_int()
{
    boost::any value = 42;
    BOOST_TEST(value.type() == boost::typeindex::type_id<int>());
}

void type_float()
{
    boost::any value = 42.f;
    BOOST_TEST(value.type() == boost::typeindex::type_id<float>());
}

void type_string()
{
    boost::any value = std::string("test message");
    BOOST_TEST(value.type() == boost::typeindex::type_id<std::string>());
}

void run()
{
    type_int();
    type_float();
    type_string();
}

} // namespace type_suite

//-----------------------------------------------------------------------------

namespace cast_suite
{

void cast_empty()
{
    boost::any value;
    BOOST_TEST_THROWS(boost::any_cast<int>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<const int>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<volatile int>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<const volatile int>(value), boost::bad_any_cast);
}

void cast_const_empty()
{
    const boost::any value;
    BOOST_TEST_THROWS(boost::any_cast<int>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<const int>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<volatile int>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<const volatile int>(value), boost::bad_any_cast);
}

void castptr_empty()
{
    boost::any value;
    BOOST_TEST_EQ(boost::any_cast<int>(&value), nullptr);
    BOOST_TEST_EQ(boost::any_cast<const int>(&value), nullptr);
    BOOST_TEST_EQ(boost::any_cast<volatile int>(&value), nullptr);
    BOOST_TEST_EQ(boost::any_cast<const volatile int>(&value), nullptr);
}

void castptr_const_empty()
{
    const boost::any value;
    BOOST_TEST_EQ(boost::any_cast<int>(&value), nullptr);
    BOOST_TEST_EQ(boost::any_cast<const int>(&value), nullptr);
    BOOST_TEST_EQ(boost::any_cast<volatile int>(&value), nullptr);
    BOOST_TEST_EQ(boost::any_cast<const volatile int>(&value), nullptr);
}

void cast_value()
{
    boost::any value = 42;
    BOOST_TEST_EQ(boost::any_cast<int>(value), 42);
    BOOST_TEST_EQ(boost::any_cast<const int>(value), 42);
    BOOST_TEST_EQ(boost::any_cast<volatile int>(value), 42);
    BOOST_TEST_EQ(boost::any_cast<const volatile int>(value), 42);

    BOOST_TEST_THROWS(boost::any_cast<int *>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<const int *>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<volatile int *>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<const volatile int *>(value), boost::bad_any_cast);

    BOOST_TEST_THROWS(boost::any_cast<float>(value), boost::bad_any_cast);
}

void cast_const_value()
{
    const boost::any value = 42;
    BOOST_TEST_EQ(boost::any_cast<int>(value), 42);
    BOOST_TEST_EQ(boost::any_cast<const int>(value), 42);
    BOOST_TEST_EQ(boost::any_cast<volatile int>(value), 42);
    BOOST_TEST_EQ(boost::any_cast<const volatile int>(value), 42);

    BOOST_TEST_THROWS(boost::any_cast<int *>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<const int *>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<volatile int *>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<const volatile int *>(value), boost::bad_any_cast);

    BOOST_TEST_THROWS(boost::any_cast<float>(value), boost::bad_any_cast);
}

void castptr_value()
{
    boost::any value = 42;
    BOOST_TEST_EQ((*boost::any_cast<int>(&value)), 42);
    BOOST_TEST_EQ((*boost::any_cast<const int>(&value)), 42);
    BOOST_TEST_EQ((*boost::any_cast<volatile int>(&value)), 42);
    BOOST_TEST_EQ((*boost::any_cast<const volatile int>(&value)), 42);

    BOOST_TEST_EQ((boost::any_cast<int *>(&value)), nullptr);
    BOOST_TEST_EQ((boost::any_cast<const int *>(&value)), nullptr);
    BOOST_TEST_EQ((boost::any_cast<volatile int *>(&value)), nullptr);
    BOOST_TEST_EQ((boost::any_cast<const volatile int *>(&value)), nullptr);

    BOOST_TEST_EQ(boost::any_cast<float>(&value), nullptr);
}

void castptr_const_value()
{
    const boost::any value = 42;
    BOOST_TEST_EQ((*boost::any_cast<int>(&value)), 42);
    BOOST_TEST_EQ((*boost::any_cast<const int>(&value)), 42);
    BOOST_TEST_EQ((*boost::any_cast<volatile int>(&value)), 42);
    BOOST_TEST_EQ((*boost::any_cast<const volatile int>(&value)), 42);

    BOOST_TEST_EQ((boost::any_cast<int *>(&value)), nullptr);
    BOOST_TEST_EQ((boost::any_cast<const int *>(&value)), nullptr);
    BOOST_TEST_EQ((boost::any_cast<volatile int *>(&value)), nullptr);
    BOOST_TEST_EQ((boost::any_cast<const volatile int *>(&value)), nullptr);

    BOOST_TEST_EQ(boost::any_cast<float>(&value), nullptr);
}

void cast_value_ref()
{
    boost::any value = 42;
    BOOST_TEST_EQ(boost::any_cast<int&>(value), 42);
    BOOST_TEST_EQ(boost::any_cast<const int&>(value), 42);
    BOOST_TEST_EQ(boost::any_cast<volatile int&>(value), 42);
    BOOST_TEST_EQ(boost::any_cast<const volatile int&>(value), 42);

    BOOST_TEST_THROWS(boost::any_cast<int *&>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<const int *&>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<volatile int *&>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<const volatile int *&>(value), boost::bad_any_cast);

    BOOST_TEST_THROWS(boost::any_cast<float&>(value), boost::bad_any_cast);
}

void cast_const_value_ref()
{
    // Casting non-const types must fail to compile

    const boost::any value = 42;
    // BOOST_TEST_EQ(boost::any_cast<int&>(value), 42);
    BOOST_TEST_EQ(boost::any_cast<const int&>(value), 42);
    // BOOST_TEST_EQ(boost::any_cast<volatile int&>(value), 42);
    BOOST_TEST_EQ(boost::any_cast<const volatile int&>(value), 42);

    // BOOST_TEST_THROWS(boost::any_cast<float&>(value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<const float&>(value), boost::bad_any_cast);
}

void castptr_value_ref()
{
    // Casting reference types must fail to compile

    boost::any value = 42;
    // BOOST_TEST_EQ(*(boost::any_cast<int&>(&value)), 42);

    // This matches the any_cast(any&&) overload
    BOOST_TEST_THROWS(boost::any_cast<const int&>(&value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<int&&>(&value), boost::bad_any_cast);
}

void castptr_const_value_ref()
{
    // Casting reference types must fail to compile

    const boost::any value = 42;
    // BOOST_TEST_EQ(*(boost::any_cast<int&>(&value)), 42);

    // This matches the any_cast(any&&) overload
    BOOST_TEST_THROWS(boost::any_cast<const int&>(&value), boost::bad_any_cast);
    BOOST_TEST_THROWS(boost::any_cast<int&&>(&value), boost::bad_any_cast);
}

void run()
{
    cast_empty();
    cast_const_empty();
    castptr_empty();
    castptr_const_empty();

    cast_value();
    cast_const_value();
    castptr_value();
    castptr_const_value();

    cast_value_ref();
    cast_const_value_ref();
    castptr_value_ref();
    castptr_const_value_ref();
}

} // namespace cast_suite

//-----------------------------------------------------------------------------

int main()
{
    api_suite::run();
    type_suite::run();
    cast_suite::run();

    return boost::report_errors();
}
