//
// Copyright (C) 2017, Jason S. McMullan <jason.mcmullan@gmail.com>
// All rights reserved.
//
// Licensed under the MIT License:
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//

auger_shaft = 20;
auger_length = 70;
auger_d = 50;
auger_width = auger_d / 14;
auger_core = 12;
auger_slices = 4;

// grub screw
grub_d = 3;
tolerance = 0.125;

// Maximum edge-to-edge of the auger blades
auger_clearance=15;

wall = 4;
fn=100;

plated = true;

gearbox_d = 32.5;

module gearbox_driven(h = 5, cut=false)
{
    if (!cut)
    {
        cylinder(d = 32.5, h = h, $fn = 120);
    }
    else
    {
        for (d = [0:2]) {
            rotate([0, 0, 120*d]) translate([12, 0, -0.1]) {
                cylinder(d=6.5, h =h + 0.2, $fn=60);
                translate([0, 0, h]) sphere(d=6.5, $fn=60);
            }
            rotate([0, 0, 60 + 120*d]) translate([12, 0, -0.1]) cylinder(d=3.5, h=h + 0.2, $fn=60);
            rotate([0, 0, 60 + 120*d]) translate([12, 0, 3-0.1]) cylinder(r=5.5/sqrt(3) + tolerance, h=3 + 0.2, $fn=6);
            rotate([0, 0, 60 + 120*d]) hull()
                {
                    translate([12.5, 0, h]) cylinder(d = 9, h = auger_shaft + 100);
                    translate([15, 0, h]) cylinder(d = 9, h = auger_shaft + 100);
                }
        }
        translate([0, 0, -0.1]) cylinder(d=5+tolerance*2, h = auger_shaft + 0.2,$fn=fn);
    }
}
        

module auger_spiral(arc=60)
{
    clearance = auger_clearance*2;
    linear_extrude(height = auger_length, center = false, convexity = 10, twist = arc * (auger_length / clearance), $fn = fn)
    {
        intersection() {
            translate([0, -auger_width/2]) square([auger_d, auger_width]);
            circle(d=auger_d);
        }
    }
}

spike_wall = 3;
spike_faces = 6;

    
module auger_of(cut=false)
{
    if (!cut)
    {
        // Spiral
        for (i = [0:auger_slices-1]) {
            clearance = auger_clearance*2;
            arc = 360/auger_slices;
            rotate([0, 0, arc*i]) {
                auger_spiral(arc);
                * for (a = [0:arc]) {
                    angle = arc - a;
                    translate([0, 0, clearance/arc*a])
                    linear_extrude(height = clearance/arc + 0.01, center=false)
                    {
                        intersection() {
                            polygon([[0,0], [auger_d/2, 0], [auger_d/2, sin(angle)*auger_d/2], [cos(arc)*auger_d/2, sin(angle)*auger_d/2]]);
                            circle(d=auger_d - tolerance*4);
                        }
                    }
                }
            }
        }
        cylinder(d1=auger_d+2, d2=auger_d, h = 2, $fn=fn);
        
        // Auger core
        cylinder(d1=auger_d, d2=auger_core, h = auger_length, $fn=fn);
    }
    else
    {
        // M5 drill
        translate([0, 0, -0.1]) cylinder(d = 5 + tolerance*2, h = auger_length + 0.2, $fn=fn);
        
        // Cone drill
        translate([0, 0, -15-0.5]) cylinder(d1=gearbox_d/2+0.5, d2=auger_core+0.5, h = auger_length - spike_wall, $fn=spike_faces);
    }
        
}

m5_nut_flat = 8;
m5_nut_h = 4;

m5_length = 20;

module auger_spike_of(cut=false)
{
    if (!cut)
    {
        // Cone drill
        cylinder(d1=gearbox_d/2, d2=auger_core, h = auger_length - spike_wall, $fn=spike_faces);
        hull()
        {
            cylinder(d=gearbox_d, h = 0.1, $fn=4);
            // Motor attachment
            translate([0, 0, -auger_shaft]) gearbox_driven(h=5, cut=cut);
        }
    } else {
        // M5 drill
        translate([0, 0, -0.1]) cylinder(d = 5 + tolerance*2, h = auger_length + 0.2, $fn=fn);
        
        // M5 nut slot
        rotate([0, 0, 0]) {
            translate([0, 0, auger_length - m5_length + spike_wall]) {
                cylinder(r = m5_nut_flat / sqrt(3), h=m5_nut_h, $fn = 6);
                translate([-20, -m5_nut_flat/2]) cube([20, m5_nut_flat, m5_nut_h]);
            }
        }
       
        // Motor attachment
        translate([0, 0, -auger_shaft]) gearbox_driven(h=5, cut=cut);
    }
}

module auger_spike()
{
    difference()
    {
        auger_spike_of(cut=false);
        auger_spike_of(cut=true);
    }
}

module auger()
{
    difference()
    {
        auger_of(cut=false);
        auger_of(cut=true);
    }
}

module auger_cap_plate()
{
    difference()
    {
        union()
        {
            cylinder(d1=auger_d+2, d2=auger_d, h = 2, $fn=fn);
            cylinder(d=auger_d, d2=auger_d, h = 2 + 1, $fn=fn);
        }
        translate([0, -(auger_d+5)/2, -1]) cube([auger_d+5, auger_d+5, 1+3+1]);
    }
}

if (!plated)
{
    rotate([0, 0, 30]) auger();
    # difference()
    {
        cylinder(d=auger_d + wall*2, h=auger_length);
        translate([0, 0, -1]) cylinder(d=auger_d, h=auger_length+2);
        translate([0, 0, auger_length - (auger_clearance + auger_d/2)]) rotate([0, 90, 0]) cylinder(d=auger_d, h = 100);
    }

    translate([0, 0, auger_length+2]) rotate([0, 180, 270]) auger_cap_plate();
}
else // projection(cut=true) translate([0, 0, -(auger_length - m5_length + m5_nut_h /2)])
{
    auger();
    
    * translate([auger_d*1.5, 0, 0])
        auger_spike();
    * translate([0, auger_d+5, 0]) auger_cap_plate();
}

// vim: set shiftwidth=4 expandtab: //
