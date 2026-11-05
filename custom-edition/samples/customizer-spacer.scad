// SPDX-License-Identifier: AGPL-3.0-or-later
/* [Afmetingen] */
// Buitendiameter
diameter = 24; // [12:1:60]
// Dikte
thickness = 6; // [1:0.5:12]
// Gatdiameter
bore = 6; // [2:1:10]
/* [Hidden] */
$fn = 64;
difference() {
    cylinder(d=diameter, h=thickness);
    translate([0, 0, -1]) cylinder(d=bore, h=thickness+2);
}
