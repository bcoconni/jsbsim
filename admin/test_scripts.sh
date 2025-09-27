#!/bin/bash
for filename in scripts/*.xml; do
[ -e "$filename" ] || continue
[ "$filename" != "scripts/kml_output.xml" ] || continue
[ "$filename" != "scripts/plotfile.xml" ] || continue
[ "$filename" != "scripts/unitconversions.xml" ] || continue
xmllint --noout --schema scripts/JSBSimScript.xsd "$filename"
done

xmllint --noout --schema scripts/JSBSimScript.xsd aircraft/Submarine_Scout/takeoff.xml
