/// @file

#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"
#include "pav_analysis.h"

using namespace std;

/// Name space of UPC
namespace upc {
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {
    unsigned int l = 0;
    r[l]= 0.0F;//o no hace falta??

    for (unsigned int l = 0; l < r.size(); ++l) {
  		/// \TODO Compute the autocorrelation r[l]
      /// \DONE Autocorrelación 
      /// - Inicializmos la autocorrelación a 0
      /// - Acumulamos los productos cruzados \f$\sum_{n=0}^{N-l} x[n]x[n+l]\f$
      /// - Dividimos por el número de muestras

      for(unsigned int n = 0; n < x.size()-l; ++n){//no multiplicaciones finales
        r[l] += x[n]*x[n+l];
      }
      //de los apuntes de clase:
      r[l]=r[l]/x.size();
      
    }

    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 
  }

  void PitchAnalyzer::set_window(Window win_type) {
    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type) {
    case HAMMING:
      /// \TODO Implement the Hamming window
      /// \DONE Hamming Window
      for (unsigned int n = 0; n < frameLen; n++){
        window[n] = 0.53836-0.46164*cos((2*M_PI*n)/(frameLen-1));
      }
    break;
    case RECT:
    default:
      window.assign(frameLen, 1);
    }
  }

  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0) {
    npitch_min = (unsigned int) samplingFreq/max_F0;
    if (npitch_min < 2)
      npitch_min = 2;  // samplingFreq/2

    npitch_max = 1 + (unsigned int) samplingFreq/min_F0;

    //frameLen should include at least 2*T0
    if (npitch_max > frameLen/2)
      npitch_max = frameLen/2;
  }

  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm, float zcr) const {
    /// \TODO Implement a rule to decide whether the sound is voiced or not.
    /// * You can use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.
    //true si sorda
    
    //Buscar que valores suelen tomar cuando se trata de sonidos sonoros (valores altos)
   
    std::cout << "POTENCIA" << pot << std::endl;
   
    if((pot>this->u_pot || r1norm>this->u_r1 ) && rmaxnorm>this->u_rmax && zcr <2200){//ajustar pot --> normalizar (máximo 0dB)
      return false;
    }else{
      return true;
    }
  }

  float PitchAnalyzer::compute_pitch(vector<float> & x) const {
    if (x.size() != frameLen)
      return -1.0F;
    //Obtain ZCR:
    //zcr = compute_zcr(x, x.size(), float fm);

    //Window input frame
    for (unsigned int i=0; i<x.size(); ++i){
      x[i] *= window[i];
    }
    //Crea un vector de tipo float llamdo r de longitud npitch_max
    vector<float> r(npitch_max);

    //Compute correlation
    autocorrelation(x, r);

    vector<float>::const_iterator iR = r.begin(), iRMax = iR;//max autocorrelación (en el 0)

    /// \TODO 
	/// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
	/// Choices to set the minimum value of the lag are:
	///    - The first negative value of the autocorrelation.
	///    - The lag corresponding to the maximum value of the pitch. -> con esto
    ///	   .
	/// In either case, the lag should not exceed that of the minimum value of the pitch.

  //for(iR =r.begin();(max_element(0,iR) == 0) && iR != iR.end();iR++);// encontrar primer mínimo??
  //if (iR == r.end()|| iR<npitch_min) return -1.0F; // no se encuetran valores negativos
  // lo segundo de arriba es por lo de "In either..."--> pero no sería con el valor dónde está el máx?
  //REVISAR LA CONDICIÓN DE ARRIBA!!!
  //probar con max_element

  /*
  Empezamos el for() tomando los valores iR e iRMax el valor del elemento que se encuentra en la 
  posición npitch_min del vector r.
    r.begin(): devuelve el valor del primer elemento del vector
    Bucle hasta que: iR tome la posición del último elemento del vector r
    iR avanza una posición en cada iteración

    Cuando el contenido del elemento que apunta iR sea mayor que el contenido apuntado por iRMax
    se asignará a iRMax la dirección (puntero) del elemento apuntado por iR
  */
  //for (iR = iRMax = r.begin() + npitch_min; iR < r.begin() + npitch_max; iR++) {
  for (iR = iRMax = r.begin() + npitch_min; iR <= r.begin() + npitch_max; iR++) {
      if (*iR > *iRMax) {
        iRMax = iR;
      }
  }
    
    unsigned int lag = iRMax - r.begin();
   
    float pot = 10 * log10(r[0]) - this->pot_max;// potencia normalizada 
    
    cout << "POTENCIA\n\n";

//ZCR --> INTENTAR CON LA FUNCIÓN
   // float zcr = compute_zcr(&x[0], x.size(), samplingFreq);
  float ZCR = 0;
  unsigned int N = x.size();
  for(unsigned int n=1; n<N; n++){
    if(x[n]*x[n-1]<0){
      ZCR++;
    }
  }
  ZCR = samplingFreq*ZCR/(2*(N-1));

  
    //You can print these (and other) features, look at them using wavesurfer
    //Based on that, implement a rule for unvoiced
    //change to #if 1 and compile
#if 0
    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << endl;
#endif
    
    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0],ZCR))
      return 0;
    else
      return (float) samplingFreq/(float) lag;
  }
}
