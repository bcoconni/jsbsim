#include <array>
#include <random>
#include <vector>

#include <cxxtest/TestSuite.h>
#include <FGFDMExec.h>
#include <math/FGFunction.h>
#include "TestUtilities.h"

using namespace JSBSim;

std::default_random_engine generator(0);
std::uniform_real_distribution<double> uniform_random(-1.0, 1.0);


class FGFunctionTest : public CxxTest::TestSuite
{
public:
  void testDefaultConstructor() {
    FGFunction f;
    TS_ASSERT(f.IsConstant());
    TS_ASSERT_EQUALS(f.GetName(), "");
    TS_ASSERT_IS_INFINITE(f.GetValue());
    f.cacheValue(true);
    TS_ASSERT_IS_INFINITE(f.GetValue());
    f.cacheValue(false);
    TS_ASSERT_IS_INFINITE(f.GetValue());
    TS_ASSERT_EQUALS(f.GetValueAsString(), "     -inf");
  }

  void testConstructorWithPropertyManager() {
    auto pm = make_shared<FGPropertyManager>();
    FGFunction f(pm);
    TS_ASSERT(f.IsConstant());
    TS_ASSERT_EQUALS(f.GetName(), "");
    TS_ASSERT_IS_INFINITE(f.GetValue());
    f.cacheValue(true);
    TS_ASSERT_IS_INFINITE(f.GetValue());
    f.cacheValue(false);
    TS_ASSERT_IS_INFINITE(f.GetValue());
    TS_ASSERT_EQUALS(f.GetValueAsString(), "     -inf");
  }

  void testProperty() {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    auto x = pm->GetNode("x", true);
    auto test = pm->GetNode("test", true);
    const array<string, 2> XML {"<dummy name=\"test\"><property>x</property></dummy>",
                                "<dummy name=\"test\"><p>x</p></dummy>"};

    for(const string& line: XML){
      Element_ptr elm = readFromXML(line);
      FGFunction f(&fdmex, elm);

      x->setAttribute(SGPropertyNode::WRITE, true);
      x->setDoubleValue(1.0);
      TS_ASSERT(!f.IsConstant());
      TS_ASSERT_EQUALS(f.GetName(), "test");
      TS_ASSERT_EQUALS(f.GetValue(), 1.0);
      TS_ASSERT_EQUALS(test->getDoubleValue(), 1.0);
      x->setDoubleValue(2.0);
      TS_ASSERT_EQUALS(f.GetValue(), 2.0);
      TS_ASSERT_EQUALS(test->getDoubleValue(), 2.0);
      f.cacheValue(true);
      x->setDoubleValue(-1.0);
      TS_ASSERT_EQUALS(f.GetValue(), 2.0);
      TS_ASSERT_EQUALS(test->getDoubleValue(), 2.0);
      f.cacheValue(false);
      TS_ASSERT_EQUALS(f.GetValue(), -1.0);
      TS_ASSERT_EQUALS(test->getDoubleValue(), -1.0);
      TS_ASSERT_EQUALS(f.GetValueAsString(), "       -1");
      // Check that read only properties are making the function constant.
      x->setAttribute(SGPropertyNode::WRITE, false);
      TS_ASSERT(f.IsConstant());
    }
  }

  void testPropertyWithPrefix() {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    FGPropertyNode_ptr test[] {pm->GetNode("test", true), pm->GetNode("test[1]", true)};
    FGPropertyNode_ptr x[] {pm->GetNode("x", true), pm->GetNode("x[1]", true)};
    const array<string, 2> XML {"<dummy name=\"test[#]\"><property>x[#]</property></dummy>",
                                "<dummy name=\"test[#]\"><p>x[#]</p></dummy>"};

    for(const string& line: XML){
      Element_ptr elm = readFromXML(line);
      for(unsigned int i=0; i<2; ++i) {
        FGFunction f(&fdmex, elm, to_string(i));

        x[i]->setDoubleValue(1.0);
        TS_ASSERT(!f.IsConstant());
        TS_ASSERT_EQUALS(f.GetName(), replace("test[#]", "#", to_string(i)));
        TS_ASSERT_EQUALS(f.GetValue(), 1.0);
        TS_ASSERT_EQUALS(test[i]->getDoubleValue(), 1.0);
        x[i]->setDoubleValue(2.0);
        TS_ASSERT_EQUALS(f.GetValue(), 2.0);
        TS_ASSERT_EQUALS(test[i]->getDoubleValue(), 2.0);
        f.cacheValue(true);
        x[i]->setDoubleValue(-1.0);
        TS_ASSERT_EQUALS(f.GetValue(), 2.0);
        TS_ASSERT_EQUALS(test[i]->getDoubleValue(), 2.0);
        f.cacheValue(false);
        TS_ASSERT_EQUALS(f.GetValue(), -1.0);
        TS_ASSERT_EQUALS(test[i]->getDoubleValue(), -1.0);
        TS_ASSERT_EQUALS(f.GetValueAsString(), "       -1");
      }
    }
  }

  void testValue() {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    auto test = pm->GetNode("test", true);
    const array<string, 2> XML {"<dummy name=\"test\"><value>-1.2</value></dummy>",
                                "<dummy name=\"test\"><v>-1.2</v></dummy>"};

    for(const string& line: XML){
      Element_ptr elm = readFromXML(line);
      FGFunction f(&fdmex, elm);

      TS_ASSERT(f.IsConstant());
      TS_ASSERT_EQUALS(f.GetName(), "test");
      TS_ASSERT_EQUALS(f.GetValue(), -1.2);
      TS_ASSERT_EQUALS(test->getDoubleValue(), -1.2);
      f.cacheValue(true);
      TS_ASSERT_EQUALS(f.GetValue(), -1.2);
      TS_ASSERT_EQUALS(test->getDoubleValue(), -1.2);
      f.cacheValue(false);
      TS_ASSERT_EQUALS(f.GetValue(), -1.2);
      TS_ASSERT_EQUALS(test->getDoubleValue(), -1.2);
      TS_ASSERT_EQUALS(f.GetValueAsString(), "     -1.2");
    }
  }

  void testPi() {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    auto out = pm->GetNode("pi_const", true);
    Element_ptr elm = readFromXML("<dummy name=\"pi_const\"><pi/></dummy>");
    FGFunction f(&fdmex, elm);

    TS_ASSERT(f.IsConstant());
    TS_ASSERT_EQUALS(f.GetName(), "pi_const");
    TS_ASSERT_EQUALS(f.GetValue(), M_PI);
    TS_ASSERT_EQUALS(out->getDoubleValue(), M_PI);
    f.cacheValue(true);
    TS_ASSERT_EQUALS(f.GetValue(), M_PI);
    TS_ASSERT_EQUALS(out->getDoubleValue(), M_PI);
    f.cacheValue(false);
    TS_ASSERT_EQUALS(f.GetValue(), M_PI);
    TS_ASSERT_EQUALS(out->getDoubleValue(), M_PI);
    TS_ASSERT_EQUALS(f.GetValueAsString(), "  3.14159");
  }

  // This function tests a <function> for any combination of <value> and
  // <property> supplied to it with a maximum of `max_nparams`.
  template<typename func_t>
  void verifyFunction(const string& func_name, const func_t& func,
                      const unsigned int max_nparams, const bool positive=false) {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    std::vector<FGPropertyNode_ptr> x{ pm->GetNode("x", true) };
    unsigned int nbelm=0;

    if (max_nparams == 2) {
      nbelm = 1;
      x.push_back(pm->GetNode("x[1]", true));
    }

    for(; nbelm<max_nparams; ++nbelm) {
      for(unsigned int i=0; i<static_cast<unsigned int>(1<<x.size()); ++i) {
        string XML = replace("<dummy name=\"test\">"
                              "<@ name=\"out\">", "@", func_name);
        double x0 = positive ? 1.0 : -1.0;
        for(unsigned int j=0; j<x.size(); ++j) {
          x[j]->setAttribute(SGPropertyNode::WRITE, true);
          x[j]->setDoubleValue(x0+2.0*j);
          x[j]->setAttribute(SGPropertyNode::WRITE, (i >> j) & 1);
          if (x[j]->getAttribute(SGPropertyNode::WRITE))
            XML.append(replace("<p>x[#]</p>", "#", to_string(j)));
          else
            XML.append(replace("<v>#</v>", "#", to_string(x[j]->getDoubleValue())));
        }
        XML.append("</@>"
                  "</dummy>");

        Element_ptr elm = readFromXML(replace(XML, "@", func_name));
        FGFunction f(&fdmex, elm);
        auto test = pm->GetNode("test", false);
        auto out = pm->GetNode("out", false);
        TS_ASSERT_EQUALS(f.IsConstant(), i == 0);
        TS_ASSERT_EQUALS(f.GetName(), "test");

        for(unsigned iter=0; iter<5; ++iter) {
          double result = func(x);

          TS_ASSERT_EQUALS(f.GetValue(), result);
          TS_ASSERT_EQUALS(test->getDoubleValue(), result);
          TS_ASSERT_EQUALS(out->getDoubleValue(), result);

          for (auto& _x: x) {
            double value = uniform_random(generator);
            if (positive) value -= uniform_random.min();
            _x->setDoubleValue(value);
          }
        }
      }
      x.push_back(pm->GetNode(replace("x[#]", "#", to_string(nbelm+1)), true));
    }
  }

  void testProduct() {
    verifyFunction("product", [](const vector<FGPropertyNode_ptr>& v) -> double {
      double result = 1.0;
      for(auto& x: v) result *= x->getDoubleValue();
      return result;
    }, 4);
  }

  void testSum() {
    verifyFunction("sum", [](const vector<FGPropertyNode_ptr>& v) -> double {
      double result = 0.0;
      for(auto& x: v) result += x->getDoubleValue();
      return result;
    }, 4);
  }

  void testAverage() {
    verifyFunction("avg", [](const vector<FGPropertyNode_ptr>& v) -> double {
      double result = 0.0;
      for(auto& x: v) result += x->getDoubleValue();
      return result / v.size();
    }, 4);
  }

  void testDifference() {
    verifyFunction("difference", [](const vector<FGPropertyNode_ptr>& v) -> double {
      double result = v[0]->getDoubleValue();
      for(unsigned int i=1; i<v.size(); ++i) result -= v[i]->getDoubleValue();
      return result;
    }, 4);
  }

  void testMinimum() {
    verifyFunction("min", [](const vector<FGPropertyNode_ptr>& v) -> double {
      double result = v[0]->getDoubleValue();
      for(unsigned int i=1; i<v.size(); ++i) result = min(result, v[i]->getDoubleValue());
      return result;
    }, 4);
  }

  void testMaximum() {
    verifyFunction("max", [](const vector<FGPropertyNode_ptr>& v) -> double {
      double result = v[0]->getDoubleValue();
      for(unsigned int i=1; i<v.size(); ++i) result = max(result, v[i]->getDoubleValue());
      return result;
    }, 4);
  }

  void testQuotient() {
    verifyFunction("quotient", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return v[0]->getDoubleValue() / v[1]->getDoubleValue();
    }, 2);
  }

  void testPower() {
    verifyFunction("pow", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return pow(v[0]->getDoubleValue(), v[1]->getDoubleValue());
    }, 2, true);
  }
};
