///////////////////////////////////////////////////////////////////////////////
// File: ShowerForwardNumberingScheme.cc
// Description: Numbering scheme for preshower detector
///////////////////////////////////////////////////////////////////////////////

#include "Geometry/EcalCommonData/interface/ShowerForwardNumberingScheme.h"
#include "DataFormats/EcalDetId/interface/ESDetId.h"

#include <iostream>
using namespace std;

ShowerForwardNumberingScheme::ShowerForwardNumberingScheme(int iv) : 
  EcalNumberingScheme(iv) {

  int iquad_M[40] = {5,7,10,11,13,13,14,15,16,17,17,17,18,
		     19,19,19,19,19,19,19,19,19,19,19,19,19,19,
		     18,17,17,17,16,15,
		     14,13,13,11,10,7,5};
  int iquad_m[40] = {1,1,1,1,1,1,1,1,1,1,1,1,1,
		     4,4,6,6,8,8,8,8,8,8,6,6,4,4,
		     1,1,1,1,1,1,1,1,1,1,1,1,1};

  int i;
  for(i=0; i<40; i++) {
    iquad_max[i] = iquad_M[i];
    iquad_min[i] = iquad_m[i];
  }
  
  Ncols[0] = 2*(iquad_max[0]-iquad_min[0]+1);
  Ntot[0] = Ncols[0];
  for (i=1; i<40; i++) {
    Ncols[i] = iquad_max[i]-iquad_min[i]+1;
    Ntot[i] = Ntot[i-1]+2*Ncols[i];
  }
  
  if (verbosity>0) 
    std::cout << "Creating ShowerForwardNumberingScheme" << std::endl;
}

ShowerForwardNumberingScheme::~ShowerForwardNumberingScheme() {
  if (verbosity>0) 
    std::cout << "Deleting ShowerForwardNumberingScheme" << std::endl;
}

uint32_t ShowerForwardNumberingScheme::getUnitID(const EcalBaseNumber& baseNumber) const 
{
  int level = baseNumber.getLevels();
  uint32_t intIndex = 0; 
  if (level > 0) {

    // depth index - silicon layer 1-st or 2-nd
    int layer = 0;
    if(baseNumber.getLevelName(0) == "SFSX") {
      layer = 1;
    } else if (baseNumber.getLevelName(0) == "SFSY") {
      layer = 2;
    } else {
      std::cout << "ShowerForwardNumberingScheme: Wrong name of Presh. Si."
		   << " Strip : " << baseNumber.getLevelName(0) << std::endl;
    }

    // Z index +Z = 1 ; -Z = 2
    int zside   = baseNumber.getCopyNumber("EREG");
    zside=2*(1-zside)+1;
    // wafer number
    int wafer = baseNumber.getCopyNumber(3);
    int x,y;
    findXY(layer, wafer, x, y);
    // strip number inside wafer
    int strip = baseNumber.getCopyNumber(0);
    if ( zside < 0 )
      x=41-x;
    intIndex =  ESDetId(strip, x, y, layer, zside).rawId(); 
    
    if (verbosity>1) {
      std::cout << "ShowerForwardNumberingScheme : zside " 
		<< zside << " layer " << layer << " wafer " << wafer << " X " << x << " Y "<< y
		<< " strip " << strip << " UnitID 0x" << std::hex << intIndex 
		<< std::dec << std::endl;
      for (int ich = 0; ich < level; ich++) {
	std::cout << "Name = " << baseNumber.getLevelName(ich) << " copy = " << baseNumber.getCopyNumber(ich)
		  << std::endl;
      }
    }
  }

  return intIndex;
}

void ShowerForwardNumberingScheme::findXY(const int& layer, const int& waf, int& x, int& y)  const {

  int i,ix,iy;

  y = 0;
  for (i=0; i<40; i++) {
    if (waf > Ntot[i]) y = i + 1;
  }

  x = 19 - iquad_max[y];
  x = (y==0) ? (x + waf) : (x + waf - Ntot[y-1]);

  int iq = iquad_min[y];
  if (iq != 1 && x > (Ncols[y]+19-iquad_max[y])) {
    x = x + (iq-1)*2;
  }
  
  if (layer==2) {
    ix = 39 - x;
    iy = 39 - y;
    x = iy;
    y = ix;
  }
  
  x++;
  y=( 39 - y ) +1; //To be compliant with CMSSW numbering scheme 
}
