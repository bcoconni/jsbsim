#include <array>
#include <random>
#include <vector>

#include <cxxtest/TestSuite.h>
#include <FGFDMExec.h>
#include <math/FGFunction.h>
#include <math/FGRealValue.h>
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

  void verifyValueParameter(FGFunction* f, Element* elm, FGPropertyNode* test,
                            const string& name, double value,
                            const string& value_str) {
    TS_ASSERT(f->IsConstant());
    if (elm->HasAttribute("name")) {
      TS_ASSERT_EQUALS(f->GetName(), name);
      TS_ASSERT(!test->getAttribute(SGPropertyNode::WRITE));
    } else {
      TS_ASSERT_EQUALS(f->GetName(), "");
      TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));
    }
    TS_ASSERT_EQUALS(f->GetValue(), value);
    if (elm->HasAttribute("name")) {
      TS_ASSERT_EQUALS(test->getDoubleValue(), value);
    }
    f->cacheValue(true);
    TS_ASSERT_EQUALS(f->GetValue(), value);
    if (elm->HasAttribute("name")) {
      TS_ASSERT_EQUALS(test->getDoubleValue(), value);
    }
    f->cacheValue(false);
    TS_ASSERT_EQUALS(f->GetValue(), value);
    if (elm->HasAttribute("name")) {
      TS_ASSERT_EQUALS(test->getDoubleValue(), value);
    }
    TS_ASSERT_EQUALS(f->GetValueAsString(), value_str);
  }

  void testConstructorWithParameterValue() {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    auto test = pm->GetNode("test", true);
    SGSharedPtr<FGRealValue> v = new FGRealValue(-1.2);
    const array<string, 2> XML {"<dummy/>", "<dummy name=\"test\"/>"};

    TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));

    for (const string& line: XML) {
      Element_ptr elm = readFromXML(line);
      FGFunction f(&fdmex, v, elm, "");

      verifyValueParameter(&f, elm, test, "test", -1.2, "     -1.2");
    }

    TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));
  }

  void testConstructorWithPropertyValue() {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    auto x = pm->GetNode("x", true);
    auto test = pm->GetNode("test", true);
    SGSharedPtr<FGPropertyValue> v = new FGPropertyValue(x);
    const array<string, 2> XML {"<dummy/>", "<dummy name=\"test\"/>"};

    TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));

    for (const string& line: XML) {
      Element_ptr elm = readFromXML(line);
      FGFunction f(&fdmex, v, elm, "");

      verifyPropertyParameter(&f, elm, x, test, "test");
    }

    TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));
  }

  void verifyPropertyParameter(FGFunction* f, Element* elm, FGPropertyNode* x,
                               FGPropertyNode* test, const string& name) {
    x->setDoubleValue(1.0);
    TS_ASSERT(!f->IsConstant());
    if (elm->HasAttribute("name")) {
      TS_ASSERT_EQUALS(f->GetName(), name);
      TS_ASSERT(!test->getAttribute(SGPropertyNode::WRITE));
    } else {
      TS_ASSERT_EQUALS(f->GetName(), "");
      TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));
    }
    TS_ASSERT_EQUALS(f->GetValue(), 1.0);
    if (elm->HasAttribute("name")) {
      TS_ASSERT_EQUALS(test->getDoubleValue(), 1.0);
    }
    x->setDoubleValue(2.0);
    TS_ASSERT_EQUALS(f->GetValue(), 2.0);
    if (elm->HasAttribute("name")) {
      TS_ASSERT_EQUALS(test->getDoubleValue(), 2.0);
    }
    f->cacheValue(true);
    x->setDoubleValue(-1.0);
    TS_ASSERT_EQUALS(f->GetValue(), 2.0);
    if (elm->HasAttribute("name")) {
      TS_ASSERT_EQUALS(test->getDoubleValue(), 2.0);
    }
    f->cacheValue(false);
    TS_ASSERT_EQUALS(f->GetValue(), -1.0);
    if (elm->HasAttribute("name")) {
      TS_ASSERT_EQUALS(test->getDoubleValue(), -1.0);
    }
    TS_ASSERT_EQUALS(f->GetValueAsString(), "       -1");
    // Check that read only properties are making the function constant.
    x->setAttribute(SGPropertyNode::WRITE, false);
    TS_ASSERT(f->IsConstant());
    x->setAttribute(SGPropertyNode::WRITE, true);
  }

  void testProperty() {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    auto x = pm->GetNode("x", true);
    auto test = pm->GetNode("test", true);
    const array<string, 4> XML {"<dummy><property>x</property></dummy>",
                                "<dummy><p>x</p></dummy>",
                                "<dummy name=\"test\"><property>x</property></dummy>",
                                "<dummy name=\"test\"><p>x</p></dummy>"};

    TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));

    for(const string& line: XML){
      Element_ptr elm = readFromXML(line);
      FGFunction f(&fdmex, elm);

      verifyPropertyParameter(&f, elm, x, test, "test");
    }

    TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));
  }

  void testPropertyWithPrefix() {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    FGPropertyNode_ptr test[] {pm->GetNode("test", true), pm->GetNode("test[1]", true)};
    FGPropertyNode_ptr x[] {pm->GetNode("x", true), pm->GetNode("x[1]", true)};
    const array<string, 2> XML {"<dummy name=\"test[#]\"><property>x[#]</property></dummy>",
                                "<dummy name=\"test[#]\"><p>x[#]</p></dummy>"};

    for(unsigned int i=0; i<2; ++i) {
      TS_ASSERT(test[i]->getAttribute(SGPropertyNode::WRITE));
    }

    for(const string& line: XML){
      Element_ptr elm = readFromXML(line);
      for(unsigned int i=0; i<2; ++i) {
        FGFunction f(&fdmex, elm, to_string(i));

        for(unsigned int j=0; j<2; ++j) {
          if (i == j) {
            TS_ASSERT(!test[j]->getAttribute(SGPropertyNode::WRITE));
          } else {
            TS_ASSERT(test[j]->getAttribute(SGPropertyNode::WRITE));
          }
        }

        verifyPropertyParameter(&f, elm, x[i], test[i],
                                replace("test[#]", "#", to_string(i)));
      }
    }

    for(unsigned int i=0; i<2; ++i) {
      TS_ASSERT(test[i]->getAttribute(SGPropertyNode::WRITE));
    }
  }

  void testValue() {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    auto test = pm->GetNode("test", true);
    const array<string, 4> XML {"<dummy><value>-1.2</value></dummy>",
                                "<dummy><v>-1.2</v></dummy>",
                                "<dummy name=\"test\"><value>-1.2</value></dummy>",
                                "<dummy name=\"test\"><v>-1.2</v></dummy>"};

    TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));

    for(const string& line: XML){
      Element_ptr elm = readFromXML(line);
      FGFunction f(&fdmex, elm);

      verifyValueParameter(&f, elm, test, "test", -1.2, "     -1.2");
    }

    TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));
  }

  void testPi() {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    auto out = pm->GetNode("pi_const", true);
    const array<string, 2> XML {"<dummy><pi/></dummy>",
                                "<dummy name=\"pi_const\"><pi/></dummy>"};

    TS_ASSERT(out->getAttribute(SGPropertyNode::WRITE));

    for(const string& line: XML) {
      Element_ptr elm = readFromXML(line);
      FGFunction f(&fdmex, elm);

      verifyValueParameter(&f, elm, out, "pi_const", M_PI, "  3.14159");
    }

    TS_ASSERT(out->getAttribute(SGPropertyNode::WRITE));
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
        double x0 = positive ? 0.5 : -0.5;
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

  void testToRadians() {
    verifyFunction("toradians", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return v[0]->getDoubleValue()*M_PI/180.;
    }, 1);
  }

  void testToDegrees() {
    verifyFunction("todegrees", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return v[0]->getDoubleValue()*180./M_PI;
    }, 1);
  }

  void testSquareRoot() {
    verifyFunction("sqrt", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return sqrt(v[0]->getDoubleValue());
    }, 1, true);
  }

  void testLogBase2() {
    verifyFunction("log2", [](const vector<FGPropertyNode_ptr>& v) -> double {
      const double invlog2val = 1.0/log10(2.0);
      return log10(v[0]->getDoubleValue())*invlog2val;
    }, 1, true);
  }

  void testLog() {
    verifyFunction("ln", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return log(v[0]->getDoubleValue());
    }, 1, true);
  }

  void testLogBase10() {
    verifyFunction("log10", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return log10(v[0]->getDoubleValue());
    }, 1, true);
  }

  void testSign() {
    verifyFunction("sign", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return v[0]->getDoubleValue() < 0.0 ? -1.0 : 1.0;
    }, 1);
  }

  void testExp() {
    verifyFunction("exp", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return exp(v[0]->getDoubleValue());
    }, 1);
  }

  void testAbs() {
    verifyFunction("abs", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return fabs(v[0]->getDoubleValue());
    }, 1);
  }

  void testSin() {
    verifyFunction("sin", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return sin(v[0]->getDoubleValue());
    }, 1);
  }

  void testCos() {
    verifyFunction("cos", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return cos(v[0]->getDoubleValue());
    }, 1);
  }

  void testTan() {
    verifyFunction("tan", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return tan(v[0]->getDoubleValue());
    }, 1);
  }

  void testAsin() {
    verifyFunction("asin", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return asin(v[0]->getDoubleValue());
    }, 1);
  }

  void testAcos() {
    verifyFunction("acos", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return acos(v[0]->getDoubleValue());
    }, 1);
  }

  void testAtan() {
    verifyFunction("atan", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return atan(v[0]->getDoubleValue());
    }, 1);
  }

  void testFloor() {
    verifyFunction("floor", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return floor(v[0]->getDoubleValue());
    }, 1);
  }

  void testCeil() {
    verifyFunction("ceil", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return ceil(v[0]->getDoubleValue());
    }, 1);
  }

  void testFmod() {
    verifyFunction("fmod", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return fmod(v[0]->getDoubleValue(), v[1]->getDoubleValue());
    }, 2);
  }

  void testAtan2() {
    verifyFunction("atan2", [](const vector<FGPropertyNode_ptr>& v) -> double {
      return atan2(v[0]->getDoubleValue(), v[1]->getDoubleValue());
    }, 2);
  }

  void testFraction() {
    verifyFunction("fraction", [](const vector<FGPropertyNode_ptr>& v) -> double {
      double dummy;
      return modf(v[0]->getDoubleValue(), &dummy);
    }, 1);
  }

  void testInteger() {
    verifyFunction("integer", [](const vector<FGPropertyNode_ptr>& v) -> double {
      double result;
      modf(v[0]->getDoubleValue(), &result);
      return result;
    }, 1);
  }

  void testRandom() {
    FGFDMExec fdmex;
    auto pm = fdmex.GetPropertyManager();
    auto test = pm->GetNode("test", true);
    auto out = pm->GetNode("out", true);
    const array<string, 8> XML {"<dummy><random/></dummy>",
                                "<dummy><random name=\"out\"/></dummy>",
                                "<dummy><random seed=\"17\"/></dummy>",
                                "<dummy><random name=\"out\" seed=\"23\"/></dummy>",
                                "<dummy name=\"test\"><random/></dummy>",
                                "<dummy name=\"test\"><random name=\"out\"/></dummy>",
                                "<dummy name=\"test\"><random seed=\"11\"/></dummy>",
                                "<dummy name=\"test\"><random name=\"out\" seed=\"7\"/></dummy>"};
    auto random_generator = RandomNumberGenerator(0);

    TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));
    TS_ASSERT(out->getAttribute(SGPropertyNode::WRITE));

    for(const string& line: XML) {
      Element_ptr elm = readFromXML(line);
      Element* random_el = elm->GetElement();

      if (random_el->HasAttribute("seed")) {
        unsigned int seed = atoi(random_el->GetAttributeValue("seed").c_str());
        random_generator.seed(seed);
      } else {
        pm->GetNode("simulation/randomseed", false)->setDoubleValue(0.0);
        random_generator.seed(0);
      }

      FGFunction f(&fdmex, elm);
      if (random_el->HasAttribute("name")) {
        TS_ASSERT(!out->getAttribute(SGPropertyNode::WRITE));
        TS_ASSERT_EQUALS(out->getDoubleValue(), random_generator.GetNormalRandomNumber());
      }

      TS_ASSERT(!f.IsConstant());
      if (elm->HasAttribute("name")) {
        TS_ASSERT_EQUALS(f.GetName(), "test");
        TS_ASSERT(!test->getAttribute(SGPropertyNode::WRITE));
        TS_ASSERT_EQUALS(test->getDoubleValue(), random_generator.GetNormalRandomNumber());
      } else {
        TS_ASSERT_EQUALS(f.GetName(), "");
        TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));
      }
      double value = random_generator.GetNormalRandomNumber();
      TS_ASSERT_EQUALS(f.GetValue(), value);
      if (random_el->HasAttribute("name")) {
        TS_ASSERT_EQUALS(out->getDoubleValue(), value);
      }
      if (elm->HasAttribute("name")) {
        value = random_generator.GetNormalRandomNumber();
        TS_ASSERT_EQUALS(test->getDoubleValue(), value);
      }
      if (random_el->HasAttribute("name")) {
        TS_ASSERT_EQUALS(out->getDoubleValue(), value);
      }
      double cached = random_generator.GetNormalRandomNumber();
      f.cacheValue(true);
      TS_ASSERT_EQUALS(f.GetValue(), cached);
      if (random_el->HasAttribute("name")) {
        TS_ASSERT_EQUALS(out->getDoubleValue(), cached);
      }
      if (elm->HasAttribute("name")) {
        TS_ASSERT_EQUALS(test->getDoubleValue(), cached);
      }
      f.cacheValue(false);
      value = random_generator.GetNormalRandomNumber();
      TS_ASSERT_EQUALS(f.GetValue(), value);
      if (random_el->HasAttribute("name")) {
        TS_ASSERT_EQUALS(out->getDoubleValue(), value);
      }
      if (elm->HasAttribute("name")) {
        value = random_generator.GetNormalRandomNumber();
        TS_ASSERT_EQUALS(test->getDoubleValue(), value);
      }
      if (random_el->HasAttribute("name")) {
        TS_ASSERT_EQUALS(out->getDoubleValue(), value);
      }
    }

    TS_ASSERT(test->getAttribute(SGPropertyNode::WRITE));
    TS_ASSERT(out->getAttribute(SGPropertyNode::WRITE));
  }
};
