#include <input_output/FGXMLParse.h>

JSBSim::Element_ptr readFromXML(const std::string& XML) {
  std::istringstream data(XML);
  auto Logger = std::make_shared<JSBSim::FGLogConsole>();
  JSBSim::FGXMLParse parser(Logger);
  readXML(data, parser);
  return parser.GetDocument();
}
