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
    -p, --u_pot=REAL   Umbral de potencia para la determinación de sonoro/sordo [default: -1e6]
    -1, --u_r1=REAL    Umbral de la autocorrelación de 1 para sonoro/sordo [default: 0.7]
    -m, --u_rmax=REAL  Umbral del máximo de la autocorrelación [default: 0.4]
    -c, --c_limit=REAL  Umbral máximo valor a partir del cual pasa a valer 0 la señal [default: 0.008]
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
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  float u_pot = stof(args["--u_pot"].asString());
  float u_r1 = stof(args["--u_r1"].asString());
  float u_rmax = stof(args["--u_rmax"].asString());
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

// CALCULAR LA POTENCIA DE TODA LA SEÑAL PARA NORMALIZAR
  cout<<"Potencia señal\n\n";
  float pot_max= 1e-12;
  int N = x.size();
  for(unsigned int n=0; n<N; n++){
  pot_max += x[n]*x[n];
  }
  pot_max = 10*log10(pot_max/N);
   
  //O CALCULAR POTENCIA DE CADA TRAMA Y VER LA MÁXIMA Y DIVIDIR ENTRE ESO??
  /* vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    for (unsigned int l = 0; l < r.size(); ++l) {
      for(unsigned int n = 0; n < x.size()-l; ++n){//no multiplicaciones finales
        r[l] += x[n]*x[n+l];
      }
      //de los apuntes de clase:
      r[l]=r[l]/x.size();
    }
    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 
    if(pot_max < r[0]){
      pot_max = r[0];
    }
  } */
    
   //= compute_power(&x,x.size());// se calcula el valor mayor de toda la señal y ahora comparar con las tramas


  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer:: RECT, 50, 500, u_pot, u_r1, u_rmax, c_limit, pot_max);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
    for (unsigned int i=0; i<x.size(); ++i){
      if(abs(x[i])<c_limit){
        x[i]=0;
      }
    }
    
    /// with offset
  /*  for (unsigned int i=0; i<x.size(); ++i){
      if(x[i]>c_limit){
        x[i]=x[i]-c_limit;
      }else if(x[i]<-c_limit){
        x[i]=x[i]+c_limit;
      }else{
        x[i]=0;
      }
    }
  */

  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
   // std::cout << "POTENCIA" << analyzer.pot << std::endl;
    f0.push_back(f);//el valor de f se agrega al final del vector fo
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.
  int w_mediana = 3;
  vector<float> vect(w_mediana,0);
  vector<float> f0_filtered=f0;
  for(int i=0; i<(int)f0.size()-1; i++){ // pq size()-1??
    for(int j=0; j<w_mediana; j++){
      vect[j] = f0[i+j];
    }
    sort(vect.begin(), vect.end());
    f0_filtered[i+1] = vect[1];
  }
 
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
