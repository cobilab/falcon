#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "paint.h"
#include "common.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

RgbColor HsvToRgb(HsvColor hsv)
  {
  RgbColor  rgb;
  uint8_t   region, remainder, p, q, t;

  if(hsv.s == 0)
    {
    rgb.r = hsv.v;
    rgb.g = hsv.v;
    rgb.b = hsv.v;
    return rgb;
    }

  region    =  hsv.h / 43;
  remainder = (hsv.h - (region * 43)) * 6; 

  p = (hsv.v * (255 - hsv.s)) >> 8;
  q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
  t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

  switch(region)
    {
    case 0:  rgb.r = hsv.v; rgb.g = t; rgb.b = p; break;
    case 1:  rgb.r = q; rgb.g = hsv.v; rgb.b = p; break;
    case 2:  rgb.r = p; rgb.g = hsv.v; rgb.b = t; break;
    case 3:  rgb.r = p; rgb.g = q; rgb.b = hsv.v; break;
    case 4:  rgb.r = t; rgb.g = p; rgb.b = hsv.v; break;
    default: rgb.r = hsv.v; rgb.g = p; rgb.b = q; break;
    }

  return rgb;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HsvColor RgbToHsv(RgbColor rgb)
  {
  HsvColor  hsv;
  uint8_t   rgbMin, rgbMax;

  rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? 
  rgb.g : rgb.b);
  rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? 
  rgb.g : rgb.b);

  hsv.v = rgbMax;
  if(hsv.v == 0)
    {
    hsv.h = 0;
    hsv.s = 0;
    return hsv;
    }

  hsv.s = 255 * (long) (rgbMax - rgbMin) / hsv.v;
  if(hsv.s == 0)
    {
    hsv.h = 0;
    return hsv;
    }

  if(rgbMax == rgb.r)
    hsv.h =       43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
  else if(rgbMax == rgb.g)
    hsv.h = 85  + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
  else
    hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

  return hsv;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *GetRgbColor(uint8_t hue)
  {
  RgbColor RGB;
  HsvColor HSV;
  char *color = (char *) Malloc(8 * sizeof(char));
  
  HSV.h = hue;
  HSV.s = LEVEL_SATURATION;
  HSV.v = LEVEL_VALUE;

  RGB = HsvToRgb(HSV);

  sprintf(color, "#%X%X%X", RGB.r, RGB.g, RGB.b); 

  return color;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *HeatMapColor(double lambda, char *color, COLORS *CLR){

  // CHANGE BEHAVIOUR [SENSITIVITY: NEAR LOW SIMILARITY // COMMENT 4 UNIFORM
  lambda = (1 + lambda*lambda*lambda + tanh(8*(lambda-1))) / 2; 

  double phi = 2*M_PI*(CLR->start/3+CLR->rotations*lambda);
  double lambdaGamma = pow(lambda, CLR->gamma);
  double a = CLR->hue*lambdaGamma*(1-lambdaGamma)/2;
  double R = lambdaGamma - a*0.14861*cos(phi) + a*1.78277*sin(phi);
  double G = lambdaGamma - a*0.29227*cos(phi) - a*0.90649*sin(phi);
  double B = lambdaGamma + a*1.97294*cos(phi);
 
  R = BoundDouble(0.0, R, 1.0);
  G = BoundDouble(0.0, G, 1.0);
  B = BoundDouble(0.0, B, 1.0);

  sprintf(color, "#%02X%02X%02X", (int) (R * 255), (int) (G * 255), (int) (B * 
  255));
  return color;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Painter *CreatePainter(double size, double width, double space)
  {
  Painter *Pa = (Painter *) Malloc(sizeof(Painter));  
  Pa->size    = size; 
  Pa->cx      = DEFAULT_CX;
  Pa->cy      = DEFAULT_CY;
  Pa->tx      = DEFAULT_TX;
  Pa->ty      = DEFAULT_TY;
  Pa->width   = width; 
  Pa->space   = space; 
  return Pa;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemovePainter(Painter *Pa){
  Free(Pa);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RectOval(FILE *F, double w, double h, double x, double y, char *color)
  {
  fprintf(F, "<rect "
              "style=\"fill:%s;fill-opacity:1;stroke-width:2;"
              "stroke-miterlimit:4;stroke-dasharray:none\" "
              "id=\"rectx\" "
              "width=\"%.2lf\" " 
              "height=\"%.2lf\" "
              "x=\"%.2lf\" "
              "y=\"%.2lf\" "
              "ry=\"12.5\" "
           "/>\n", color, w, h, x, y);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RectOvalIR(FILE *F, double w, double h, double x, double y, char *color)
  {
  RectOval(F, w, h, x, y, color);
  fprintf(F, "<rect "
              "style=\"fill-opacity:1;stroke-width:2;"
              "stroke-miterlimit:4;stroke-dasharray:none"
              "stroke-dasharray:none;fill:url(#xtrace);"
              "fill-rule:nonzero;opacity:1\" "
              "id=\"recty\" "
              "width=\"%.2lf\" " 
              "height=\"%.2lf\" "
              "x=\"%.2lf\" "
              "y=\"%.2lf\" "
              "ry=\"12.5\" "
           "/>\n", w, h, x, y);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Rect(FILE *F, double w, double h, double x, double y, char *color)
  {
  fprintf(F, "<rect style=\"fill:%s;fill-opacity:1;stroke-width:2;"
              "stroke-miterlimit:4;stroke-dasharray:none\" "
              "id=\"rect3777\" "
              "width=\"%.2lf\" " 
              "height=\"%.2lf\" "
              "x=\"%.2lf\" "
              "y=\"%.2lf\" "
              "ry=\"0\" "
           "/>\n", color, w, h, x, y);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RectIR(FILE *F, double w, double h, double x, double y, char *color)
  {
  Rect(F, w, h, x, y, color);
  fprintf(F, "<rect "
              "style=\"fill-opacity:1;stroke-width:2;stroke-miterlimit:4;"
              "stroke-dasharray:none;fill:url(#Wavy);fill-rule:"
              "nonzero;opacity:1\" "
              "id=\"rect6217\" "
              "width=\"%.2lf\" " 
              "height=\"%.2lf\" "
              "x=\"%.2lf\" "
              "y=\"%.2lf\" "
              "ry=\"0\" "
           "/>\n", w, h, x, y);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Chromosome(FILE *F, double w, double h, double x, double y)
  {
  char borderColor[] = "#000000";

  double wk = w / 2 + 0.5;
  fprintf(F, "<path "
         "d=\"m %.2lf,"
         "%.2lf 0,"
         "%.2lf c 0, -8.31 6.69, -%.2lf %.2lf, -%.2lf l -%.2lf,0 z m %.2lf,"
         "0 c 8.31,0 %.2lf,6.69 %.2lf,%.2lf l 0,-%.2lf -%.2lf,0 z\" "
         "id=\"rect3787\" style=\"fill:#fff;fill-opacity:1;fill-rule:"
         "nonzero;stroke:none\" />", x-0.5, y-0.5, 
         wk, wk, wk, wk, wk, wk, wk, wk, wk, wk, wk);

  fprintf(F, "<path "
         "d=\"m %.2lf,"
         "%.2lf 0,"
         "-%.2lf c 0,8.31 -6.69, %.2lf -%.2lf, %.2lf l %.2lf,0 z m -%.2lf,"
         "0 c -8.31,0 -%.2lf,-6.69 -%.2lf,-%.2lf l 0,%.2lf %.2lf,0 z\" "
         "id=\"rect3787\" style=\"fill:#fff;fill-opacity:1;fill-rule:"
         "nonzero;stroke:none\" />", x+0.5+w, y+0.5+h, 
         wk, wk, wk, wk, wk, wk, wk, wk, wk, wk, wk);

  fprintf(F, "<rect style=\"fill:none;stroke:%s;stroke-width:1;"
            "stroke-linecap:butt;stroke-linejoin:miter;"
            "stroke-miterlimit:1;stroke-opacity:1;"
            "stroke-dasharray:none\" "
            "id=\"rect2985\" "
            "width=\"%.2lf\" " 
            "height=\"%.2lf\" "
            "x=\"%.2lf\" "
            "y=\"%.2lf\" "
            "ry=\"12.5\" "     // COMMENT THIS FOR STRICT LINES
          "/>\n", borderColor, w, h, x, y);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Text90d(FILE *F, double x, double y, char *name)
  {
  fprintf(F, "<text xml:space=\"preserve\" "
            "style=\"font-size:30px;font-style:normal;"
            "font-weight:normal;line-height:125%%;"
            "letter-spacing:0px;word-spacing:0px;fill:#000000;"
            "fill-opacity:1;stroke:none;font-family:Sans\" "
            "transform=\"rotate(-90)\" "
            "x=\"%.2lf\" "
            "y=\"%.2lf\" "
            "id=\"corben\" "
            "sodipodi:linespacing=\"125%%\">"
            "<tspan sodipodi:role=\"line\" "
            "id=\"tspan3804\" "
            "x=\"%.2lf\" "
            "y=\"%.2lf\" "
            "style=\"font-size:16px;font-style:normal;font-variant:"
            "normal;font-weight:normal;font-stretch:normal;"
            "text-align:start;line-height:125%%;writing-mode"
            ":lr-tb;text-anchor:start;font-family:monospace;\">%s</tspan>\n"
         "</text>\n", x, y-4, x, y-4, name);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Text(FILE *F, double x, double y, char *name)
  {
  fprintf(F, "<text xml:space=\"preserve\" "
            "style=\"font-size:30px;font-style:normal;"
            "font-weight:normal;line-height:125%%;"
            "letter-spacing:0px;word-spacing:0px;fill:#000000;"
            "fill-opacity:1;stroke:none;font-family:Sans\" "
            "x=\"%.2lf\" "
            "y=\"%.2lf\" "
            "id=\"corben\" "
            "sodipodi:linespacing=\"125%%\">"
            "<tspan sodipodi:role=\"line\" "
            "id=\"tspan3804\" "
            "x=\"%.2lf\" "
            "y=\"%.2lf\" "
            "style=\"font-size:16px;font-style:normal;font-variant:"
            "normal;font-weight:normal;font-stretch:normal;"
            "text-align:start;line-height:125%%;writing-mode"
            ":lr-tb;text-anchor:start;font-family:monospace;\">%s</tspan>\n"
         "</text>\n", x, y, x, y, name);
  }


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void TextFloat(FILE *F, double x, double y, double name)
  {
  fprintf(F, "<text xml:space=\"preserve\" "
             "style=\"font-size:40px;font-style:normal;"
             "font-weight:normal;line-height:125%%;"
             "letter-spacing:0px;word-spacing:0px;fill:#000000;"
             "fill-opacity:1;stroke:none;font-family:Sans\" "
             "x=\"%.2lf\" "
             "y=\"%.2lf\" "
             "id=\"corben\" "
             "sodipodi:linespacing=\"125%%\">"
             "<tspan sodipodi:role=\"line\" "
             "id=\"tspan3804\" "
             "x=\"%.2lf\" "
             "y=\"%.2lf\" "
             "style=\"font-size:18px;font-style:normal;font-variant:"
             "normal;font-weight:normal;font-stretch:normal;"
             "text-align:start;line-height:125%%;writing-mode"
             ":lr-tb;text-anchor:start;font-family:Arial;-\">%5.2lf</tspan>\n"
          "</text>\n", x, y, x, y, name);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintHead(FILE *F, double w, double u)
  {
  fprintf(F, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
  "<!-- SMASH @ IEETA 2015 -->\n""<svg\n"
  "xmlns:osb=\"http://www.openswatchbook.org/uri/2009/osb\"\n"
  "xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
  "xmlns:cc=\"http://creativecommons.org/ns#\"\n"
  "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
  "xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
  "xmlns=\"http://www.w3.org/2000/svg\"\n"
  "width=\"%lf\"\n"
  "height=\"%lf\"\n"
  "id=\"svg2\"\n"
  "version=\"1.1\"\n"
  "sodipodi:docname=\"chromosomes\">\n"
  "<defs\n"
  "id=\"defs4\">\n"
    "<linearGradient id=\"linearGradient3755\" osb:paint=\"solid\">\n"
      "<stop style=\"stop-color:#aa00ff;stop-opacity:1;\" offset=\"0\"\n"
      "id=\"stop3757\" />\n"
    "</linearGradient>\n"
  "</defs>\n"
  "<sodipodi:namedview id=\"base\" pagecolor=\"#ffffff\"\n"
  "bordercolor=\"#666666\" borderopacity=\"1.0\"\n"
  "inkscape:pageopacity=\"0.0\" inkscape:pageshadow=\"2\"\n"
  "inkscape:zoom=\"0.49497475\" inkscape:cx=\"159.73474\"\n"
  "inkscape:cy=\"681.53008\" inkscape:document-units=\"px\"\n"
  "inkscape:current-layer=\"layer1\" showgrid=\"false\"\n"
  "inkscape:window-width=\"1066\" inkscape:window-height=\"790\"\n"
  "inkscape:window-x=\"278\" inkscape:window-y=\"1106\"\n"
  "inkscape:window-maximized=\"1\" />\n"
  "<metadata id=\"metadata7\">\n"
    "<rdf:RDF>\n"
      "<cc:Work rdf:about=\"\">\n"
        "<dc:format>image/svg+xml</dc:format>\n"
        "<dc:type rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\""
        " />\n"
        "<dc:title></dc:title>\n"
      "</cc:Work>\n"
    "</rdf:RDF>\n"
  "</metadata>\n"
  "<g inkscape:label=\"Camada 1\" inkscape:groupmode=\"layer\" id=\"layer1\" "
  ">\n", w, u);
  
  fprintf(F, "<defs id=\"defs4\"><pattern id=\"dallas\" patternTransform=\"tr"
  "anslate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" patternUnits"
  "=\"userSpaceOnUse\"> <path style=\"fill:none;stroke:#000000;stroke-width:1"
  ";stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:0;stroke-opac"
  "ity:1;stroke-dasharray:none\" d=\"m -0.00788,2.37557 4.76251,0\" id=\"path"
  "2985\" /><path style=\"fill:none;stroke:#000000;stroke-width:1;stroke-line"
  "cap:butt;stroke-linejoin:miter;stroke-miterlimit:0;stroke-opacity:1;stroke"
  "-dasharray:none\" d=\"m 2.37338,-0.00568 0,4.76251\" id=\"path2985-1\" /><"
  "/pattern></defs>");

  fprintf(F, "<defs id=\"defs4\"><pattern id=\"lineX\" patternTransform=\"tra"
  "nslate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" patternUnits="
  "\"userSpaceOnUse\"> <path style=\"fill:none;stroke:#000000;stroke-width:1;"
  "stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:0;stroke-opaci"
  "ty:1;stroke-dasharray:none\" d=\"m -0.00788,2.37557 4.76251,0\" id=\"path2"
  "985\" /></pattern></defs>");

  fprintf(F, "<defs id=\"defs4\"><pattern id=\"stripeX\" patternTransform=\"t"
  "ranslate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" patternUnit"
  "s=\"userSpaceOnUse\"> <path style=\"fill:none;stroke:#000000;stroke-width:"
  "1;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:0;stroke-opa"
  "city:1;stroke-dasharray:none\" d=\"m 2.37338,-0.00568 0,4.76251\" id=\"pat"
  "h2985-1\" /></pattern></defs>");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintFinal(FILE *F)
  {
  fprintf(F, "</g>\n</svg>");
  fclose(F);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
