/// @file

#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"
#include "pav_analysis.h"

using namespace std;

/// Name space of UPC
namespace upc {
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {

    for (unsigned int l = 0; l < r.size(); ++l) {
  		/// \TODO Compute the autocorrelation r[l]
      /// \DONE Autocorrelación 
      /// - Inicializmos la autocorrelación a 0
      /// - Acumulamos los productos cruzados \f$\sum_{n=0}^{N-l} x[n]x[n+l]\f$
      /// - Dividimos por el número de muestras
      r[l]=0.0F;
      for(unsigned int n = 0; n < x.size()-l; ++n){
        r[l] += x[n]*x[n+l];
      }
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
      /// - Obtención de los coeficientes de la ventana de Hamming: \f$\ w[n]=a0-a1·cos(2·pi·n/(N-1)) \f$
      ///   con \f$\ a0 = 0.53836 \f$ y \f$\ a1 = 0.46164 \f$
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
      npitch_min = 2;  

    npitch_max = 1 + (unsigned int) samplingFreq/min_F0;

    //frameLen should include at least 2*T0
    if (npitch_max > frameLen/2)
      npitch_max = frameLen/2;
  }

  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm, float zcr) const {
    /// \TODO Implement a rule to decide whether the sound is voiced or not.
    /// * You can use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.
    /// \DONE Criterios de decisión sordo/sonoro establecidos:
    /// - Potencia: si es mayor que el umbral u_pot es sonoro (potencia elevada), sino sordo
    /// - r1norm: Si el cociente r[1]/r[0] es mayor que el umbral u_r1 es sonoro, sino sordo
    /// - rmaxnorm: Si el cociente r[lag]/r[0] es mayor que el umbral u_rmax es sonoro, sino sordo
    /// - zcr: Si es inferior que el umbral u_zcr es sonoro (pocos cruces por cero), sino sordo
    /// Cuando es sonoro devuelve false y cuando es sordo devuelve true
   
    if((pot>this->u_pot || r1norm>this->u_r1 ) && rmaxnorm>this->u_rmax && zcr <u_zcr){
      return false;
    }else{
      return true;
    }
  }

  float PitchAnalyzer::compute_pitch(vector<float> & x) const {

    if (x.size() != frameLen)
      return -1.0F;

    //Window input frame
    for (unsigned int i=0; i<x.size(); ++i){
      x[i] *= window[i];
    }

    //Crea un vector de tipo float llamdo r de longitud npitch_max
    vector<float> r(npitch_max);

    //Compute correlation
    autocorrelation(x, r);

    //El máximo de la autocorrelación está en el 0
    vector<float>::const_iterator iR = r.begin(), iRMax = iR;

    /// \TODO 
    /// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
    /// Choices to set the minimum value of the lag are:
    /// - The first negative value of the autocorrelation.
    /// - The lag corresponding to the maximum value of the pitch. -> con esto.
    /// In either case, the lag should not exceed that of the minimum value of the pitch.
    /// \DONE Encontrar índice de retardo del primer máximo secundario de la autocorrelación
    /// - Creación de un bucle de tipo for:
    ///     Inicialización de iR e iRMax que apuntan al elemento que se encuentra en la 
    ///     posición npitch_min del vector r (r.begin() es el ínidce del valor del primer 
    ///     elemento del vector r).
    ///     Se itera hasta que iR toma el valor del índice del elemento que se encuentra en 
    ///     la posición del último elemento del vector r (que tiene tamaño npitch_max). En 
    ///     cada iteración, iR avanza una posición.
    /// - Uso de un condicional para almacenar el máximo secundario de la autocorrelación:
    ///     Cuando el contenido del elemento apuntado por iR sea mayor que el contenido apuntado 
    ///     por iRMax, se asignará a iRMax la dirección (puntero) del valor apuntado por iR (que 
    ///     apunta al máximo encontrado hasta el momento).
    /// - Guardar el retardo del máximo secundario en la variable lag

    for (iR = iRMax = r.begin() + npitch_min; iR <= r.begin() + npitch_max; iR++) {
      if (*iR > *iRMax) {
        iRMax = iR;
      }
    }

    unsigned int lag = iRMax - r.begin();
   
    // Normalizar la potencia respecto del máximo de potencia de la señal
    float pot = 10 * log10(r[0]) - this->pot_max; 

    // Cálculo del parámetro ZCR
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
#if 1
    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << '\t' << ZCR << endl;
#endif
    
    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0], ZCR))
      return 0;
    else
      return (float) samplingFreq/(float) lag;
  }
}
