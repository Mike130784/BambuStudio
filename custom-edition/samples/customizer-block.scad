// SPDX-License-Identifier: AGPL-3.0-or-later
/* [Maten] */
// Breedte
width = 20; // [1:1:200]
// Diepte
depth = 10; // [1:200]
// Hoogte
height = 5; // [1:200]
/* [Hidden] */
internal_scale = 1;
cube([width, depth, height] * internal_scale);
