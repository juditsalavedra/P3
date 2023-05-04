/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "pav_analysis.h"///////////añadido!!!!!
#include <math.h>///////////añadido!!!!!

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -p, --u_pot=REAL   Umbral de potencia para la determinación de sonoro/sordo [default: -30]
    -1, --u_r1=REAL    Umbral de la autocorrelación de 1 para sonoro/sordo [default: 0.6]
    -m, --u_rmax=REAL  Umbral del máximo de la autocorrelación [default: 0.4]
    -z, --u_zcr=REAL   Umbral de zcr para la determinación de sonoro/sordo [default: 2200]
    -c, --c_limit=REAL  Umbral máximo valor a partir del cual pasa a valer 0 la señal [default: 0.007]
    -h, --help  Show this screen
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
  /// \DONE Modificación docopt.
  ///  Hemos añadido las siguientes opciones/argumentos al programa para la 
  ///  determinación de sonoridad y preprocesado:
  /// - u_pot: Umbral de nivel de potencia
  /// - u_r1: Umbral de autocorrelación normalizada
  /// - u_rmax: Umbral de autocorrelación en su máximo secundario
  /// - u_zcr: Umbral de tasa de cruces por cero (ZCR) 
  /// - c_limit: Umbral para recortar la señal (center clipping)

    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  float u_pot = stof(args["--u_pot"].asString());
  float u_r1 = stof(args["--u_r1"].asString());
  float u_rmax = stof(args["--u_rmax"].asString());
  float u_zcr = stof(args["--u_zcr"].asString());
  float c_limit = stof(args["--c_limit"].asString());

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

// Calculamos la potencia de la señal para normalizar
  float pot_max= 1e-12;
  unsigned int N = x.size();
  for(unsigned int n=0; n<N; n++){
    pot_max += x[n]*x[n];
  }
  pot_max = 10*log10(pot_max/N);


  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer:: RECT, 50, 500, u_pot, u_r1, u_rmax, u_zcr, c_limit, pot_max);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  /// \DONE Central-clipping (sin offset)
  /// - Uso del bucle for: Se mira el valor de cada muestra de la señal x.
  /// - Si el valor absoluto de la muestra es menor que el umbral establecido c_limit,
  ///   se le asigna el valor 0. En caso de ser mayor que el umbral, mantiene su valor.

  for (unsigned int i=0; i<x.size(); ++i){
    if(abs(x[i])<c_limit){
      x[i]=0;
    }
  }
    
  // with offset
    // for (unsigned int i=0; i<x.size(); ++i){
    //   if(x[i]>c_limit){
    //     x[i]=x[i]-c_limit;
    //   }else if(x[i]<-c_limit){
    //     x[i]=x[i]+c_limit;
    //   }else{
    //     x[i]=0;
    //   }
    // }
  

  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);//el valor de f se agrega al final del vector fo
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.
  /// \DONE Median filter
  /// - Definimos el tamaño de la ventana usada: 3
  /// - Creación de dos vectores:
  ///        vect: en cada iteración se guardan los tres valores de f0 qu se encuentran debajo 
  ///           del filtro centrado en la posición i-ésima.
  ///        f0_ini: se inicializa con los valores de los dos primeros elementos de f0, ya que
  ///             el filtro empieza centrado en la posición i=0. Como no hay muestra en i=-1, 
  ///             la primera iteración la realizamos considerando que en i=-1 el valor es 0.
  /// - Se ordenan ascendentemente los tres valores de f0_ini.
  /// - Definimos un vector f0_filtered para ir guardando los valores resultantes de aplicar 
  ///   el filtro de mediana. Al ser un filtro no recursivo, no podemos modificar f0, ya que 
  ///   la salida del filtro tiene que ser calculada usando únicamente los valores de entrada
  ///   (no realimentación). 
  /// - El valor del primer elemento obtenido al aplicar el filtro de mediana es el valor que
  ///   se encuentra en el índice central de f0_ini.
  /// - Se crea un bucle de tipo for() para iterar a lo largo de f0 (contiene los valores de pitch 
  ///   obtenidos). Empieza en la posición i = 0 (filtro centrado en i=1), ya que ya hemos calculado 
  ///   la salida para el caso en el que el filtro está centrado en la primera muestra. Termina en 
  ///   i = # de elementos de f0 -2 (filtro centrado en # elementos de f0 -1).
  /// - Se crea otro bucle de tipo for() para asignar los valores correspondientes a vect.
  /// - Se ordenan ascendentemente los tres valores que contiene vect.
  /// - Se asigna al elemento i-ésimo de f0_filtered el valor mediano.
  /// - Se repite lo mismo que se ha hecho para el caso de filtro centrado en el primer elemento para
  ///   el caso de filtro centrado en el último elemento

  int w_mediana = 3;

  vector<float> vect(w_mediana,0);
  vector<float> f0_ini = {0, f0[0], f0[1]};
  sort(f0_ini.begin(), f0_ini.end());
  vector<float> f0_filtered=f0;

  f0_filtered[0] = f0_ini[1];
  for(int i=0; i<(int)f0.size()-1; i++){ 
    for(int j=0; j<w_mediana; j++){
      vect[j] = f0[i+j];
    }
    sort(vect.begin(), vect.end());
    f0_filtered[i+1] = vect[1];
  }

  vector<float> f0_fin =  {f0[(int)f0.size()-2], f0[(int)f0.size()-1], 0};
  sort(f0_fin.begin(), f0_fin.end());
  f0_filtered[(int)f0.size()-1] = f0_fin[1];
 

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0_filtered.begin(); iX != f0_filtered.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}
