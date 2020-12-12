#include <cxxtest/TestSuite.h>
#include <math/FGPropertyValue.h>

using namespace JSBSim;

class FGPropertyValueTest : public CxxTest::TestSuite
{
public:
  void testConstructorFromNode() {
    FGPropertyNode root;
    FGPropertyNode_ptr node = root.GetNode("x", true);
    FGPropertyValue property(node);

    TS_ASSERT_EQUALS(property.GetValue(), 0.0);
    TS_ASSERT_EQUALS(property.IsConstant(), false);
    TS_ASSERT_EQUALS(property.IsLateBound(), false);
    TS_ASSERT_EQUALS(property.GetName(), std::string("x"));
    TS_ASSERT_EQUALS(property.GetNameWithSign(), std::string("x"));
    TS_ASSERT_EQUALS(property.GetFullyQualifiedName(), std::string("/x"));
    TS_ASSERT_EQUALS(property.GetPrintableName(), std::string("x"));
  }

  void testSetValue() {
    FGPropertyNode root;
    FGPropertyNode_ptr node = root.GetNode("x", true);
    FGPropertyValue property(node);

    TS_ASSERT_EQUALS(node->GetDouble(), 0.0);
    property.SetValue(1.54);
    TS_ASSERT_EQUALS(property.GetValue(), 1.54);
    TS_ASSERT_EQUALS(node->GetDouble(), 1.54);
  }

  void testSetNode() {
    FGPropertyNode root;
    FGPropertyNode_ptr node_x = root.GetNode("x", true);
    FGPropertyNode_ptr node_y = root.GetNode("y", true);
    FGPropertyValue property(node_x);

    node_y->SetDouble(-1.547);
    TS_ASSERT_EQUALS(property.GetValue(), 0.0);
    TS_ASSERT_EQUALS(property.GetName(), "x");
    property.SetNode(node_y);
    TS_ASSERT_EQUALS(property.GetValue(), -1.547);
    TS_ASSERT_EQUALS(property.GetName(), "y");
  }

  void testConstant_ness() {
    FGPropertyNode root;
    FGPropertyNode_ptr node = root.GetNode("x", true);
    FGPropertyValue property(node);

    TS_ASSERT_EQUALS(property.IsConstant(), false);
    #ifndef AUTOMATIC_DIFFERENTIATION
    node->setAttribute(SGPropertyNode::WRITE, false);
    TS_ASSERT_EQUALS(property.IsConstant(), true);
    #endif
  }

  void testConstructorLateBound() {
    auto pm = make_shared<FGPropertyManager>();
    FGPropertyValue property("x", pm, nullptr);

    TS_ASSERT_EQUALS(property.IsLateBound(), true);
    TS_ASSERT_EQUALS(property.GetName(), std::string("x"));
    TS_ASSERT_EQUALS(property.GetNameWithSign(), std::string("x"));
    TS_ASSERT_EQUALS(property.GetFullyQualifiedName(), std::string("x"));
    TS_ASSERT_EQUALS(property.GetPrintableName(), std::string("x"));
    TS_ASSERT_EQUALS(property.IsConstant(), false);
    TS_ASSERT_EQUALS(property.IsLateBound(), true);
  }

  void testInstantiateLateBound() {
    auto pm = make_shared<FGPropertyManager>();
    FGPropertyValue property("x", pm, nullptr);

    TS_ASSERT_EQUALS(property.IsLateBound(), true);

    auto node = pm->GetNode("x", true);
    TS_ASSERT_EQUALS(property.GetValue(), 0.0);
    TS_ASSERT_EQUALS(property.IsLateBound(), false);
    TS_ASSERT_EQUALS(property.GetName(), std::string("x"));
    TS_ASSERT_EQUALS(property.GetNameWithSign(), std::string("x"));
    TS_ASSERT_EQUALS(property.GetFullyQualifiedName(), std::string("/x"));
    TS_ASSERT_EQUALS(property.GetPrintableName(), std::string("x"));

    // Check the link is two-way.
    node->SetDouble(1.3574);
    TS_ASSERT_EQUALS(property.GetValue(), 1.3574);

    property.SetValue(-2.01);
    TS_ASSERT_EQUALS(node->GetDouble(), -2.01);
  }

  void testSignedProperty() {
    auto pm = make_shared<FGPropertyManager>();
    FGPropertyValue property("-x", pm, nullptr);

    TS_ASSERT_EQUALS(property.IsLateBound(), true);
    TS_ASSERT_EQUALS(property.GetName(), std::string("x"));
    TS_ASSERT_EQUALS(property.GetNameWithSign(), std::string("-x"));
    TS_ASSERT_EQUALS(property.GetFullyQualifiedName(), std::string("x"));
    TS_ASSERT_EQUALS(property.GetPrintableName(), std::string("x"));
    TS_ASSERT_EQUALS(property.IsConstant(), false);
    TS_ASSERT_EQUALS(property.IsLateBound(), true);

    auto node = pm->GetNode("x", true);
    node->SetDouble(1.234);
    TS_ASSERT_EQUALS(property.GetValue(), -1.234);
  }
};
