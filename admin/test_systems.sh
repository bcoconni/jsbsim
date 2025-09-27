#!/bin/bash
for filename in systems/*.xml; do
    [ -e "$filename" ] || continue
    xmllint --noout --schema systems/JSBSimSystem.xsd "$filename"
done
