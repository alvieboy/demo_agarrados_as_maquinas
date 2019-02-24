#include <math.h>
#include "hsv.h"

static float Hue_2_RGB( float v1, float v2, float vH )             //Function Hue_2_RGB
{
   if ( vH < 0 ) 
     vH += 1;
   if ( vH > 1 ) 
     vH -= 1;
   if ( ( 6 * vH ) < 1 ) 
     return ( v1 + ( v2 - v1 ) * 6 * vH );
   if ( ( 2 * vH ) < 1 ) 
     return ( v2 );
   if ( ( 3 * vH ) < 2 ) 
     return ( v1 + ( v2 - v1 ) * (.66-vH) * 6 );
   return ( v1 );
}

static void HSL(uint8_t max, float H, float S, float L, float* Rval, float*Gval, float* Bval)
{
  if (H < 0) {H = 0;}
  else if (H > 1) {
    while (H>1) {H = H - 1;}
  }
  if (S < 0) {S = 0;}
  else if (S > 1) {S = 1;}
  float var_1;
  float var_2;
  float Hu=H+.33;
  float Hd=H-.33;
  if ( S == 0 )                       //HSL from 0 to 1
  {
     *Rval = L * (float)max;                      //RGB results from 0 to 255
     *Gval = L * (float)max;
     *Bval = L * (float)max;
  }
  else
  {
     if ( L < 0.5 ) 
       var_2 = L * ( 1 + S );
     else           
       var_2 = ( L + S ) - ( S * L );
 
     var_1 = 2 * L - var_2;
 
     *Rval = round((float)max * Hue_2_RGB( var_1, var_2, Hu ));
//     Serial.print("Rval:");
//     Serial.println(Hue_2_RGB( var_1, var_2, Hu ));
     *Gval = round((float)max * Hue_2_RGB( var_1, var_2, H ));
     *Bval = round((float)max * Hue_2_RGB( var_1, var_2, Hd ));
  }
 
}

#define CLAMP(x,max) if ((x)<0) x=0; if ((x)>max) x=max

void compute_hsv(unsigned steps, uint8_t max, uint8_t *dest)
{
    int i;
    float r,g,b;
    float hue;
    float hsvalue;
    float rgain = 1; //this is green
    float ggain = 0.75; //this is red
    float bgain = 1;

    for (i=0;i<steps;i++) {
        hue = (float)i/600; //Chosen value for Mark's performnce in reds
        hsvalue = sin((float)i/(steps-1)); //This was the function used at Unify and CO.lab
        if (hue < 0) {hue = 0;}
        if (hsvalue < 0) {hsvalue = 0;}
        //HSL( (0.7+(float)log(i)/4.5), 0.99, (float)log(2*i)/10.0,r,g,b); //burning man 2012
        //		HSL( (0.7+(float)log(i)/4.5), 0.99, hsvalue,r,g,b); /255/burning man 2012
        HSL( max, (0.975 + hue), 0.99, hsvalue,&r,&g,&b); //"blue / aqua" color mapping for Mark's performance
        //		HSL( (0.975 + hue), 0.99, hsvalue,r,g,b); //"blue / aqua" color mapping for Mark's performance
        //		HSL( (0.0+(float)log(i)/5.0), 0.99, (float)log(4*i)/12.0,r,g,b); // greens and yellows
        //		HSL( (0.2+(float)log(i)/2.0), 0.99, (float)i/16.0,r,g,b);
        r = r * rgain; //swapping channels to fix the mapping
        g = g * ggain;
        b = b * bgain;
        int ur =(int)g;
        int ug =(int)r;
        int ub =(int)b;
        CLAMP(ur,max);
        CLAMP(ug,max);
        CLAMP(ub,max);

        *dest++=(unsigned)ur;
        *dest++=(unsigned)ug;
        *dest++=(unsigned)ub;
    }
}
