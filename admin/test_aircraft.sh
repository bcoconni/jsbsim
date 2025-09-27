#!/bin/bash
for dir in $(ls -d aircraft/*); do
name=$(basename "$dir")
[ "$name" != "blank" ] || continue
filename="${dir}/${name}.xml"
[ -e "$filename" ] || continue
xmllint --noout --schema JSBSim.xsd "$filename"

if [ -d "${dir}/Systems" ]; then
    for filename in "${dir}/Systems"/*.xml; do
    [[ $filename != *_aero.xml ]] || continue
    xmllint --noout --schema JSBSimSystem.xsd "$filename"
    done
fi

for filename in ${dir}/*ap{,x}.xml; do
    [ -e "$filename" ] || continue
    xmllint --noout --schema aircraft/JSBSimAutopilot.xsd "$filename"
done
done

xmllint --noout --schema JSBSimSystem.xsd aircraft/F450/Effectors.xml
xmllint --noout --schema aircraft/JSBSimFlightControl.xsd aircraft/F450/FlightControl.xml
