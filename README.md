PAV - P3: estimación de pitch
=============================

Esta práctica se distribuye a través del repositorio GitHub [Práctica 3](https://github.com/albino-pav/P3).
Siga las instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para realizar un `fork` de la
misma y distribuir copias locales (*clones*) del mismo a los distintos integrantes del grupo de prácticas.

Recuerde realizar el *pull request* al repositorio original una vez completada la práctica.

Ejercicios básicos
------------------

- Complete el código de los ficheros necesarios para realizar la estimación de pitch usando el programa
  `get_pitch`.

   * Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.
   
     A continuación se muestra el código que se ha completado para el cálculo de la autocorrelación:
     ```c
     for(unsigned int l = 0; l < r.size(); ++l) {
   	  r[l]=0.0F;
		  for(unsigned int n = 0; n < x.size()-l; ++n){
			  r[l] += x[n]*x[n+l];
		  }
		  r[l]=r[l]/x.size();
     }
   
     if (r[0] == 0.0F) //to avoid log() and divide zero
   	  r[0] = 1e-10; 
     ```

   * Inserte una gŕafica donde, en un *subplot*, se vea con claridad la señal temporal de un segmento de
     unos 30 ms de un fonema sonoro y su periodo de pitch; y, en otro *subplot*, se vea con claridad la
     autocorrelación de la señal y la posición del primer máximo secundario.
     
     Para la realización de este apartado, se ha realizado la grabación de la vocal `a` (sonido sonoro)
     mediante el uso de `wavesurfer`. Se ha recortado con el objetivo de obtener una señal de duración 
     de 30 ms aproximadamente:
     &nbsp;&nbsp;<img width="957" alt="image" src="https://user-images.githubusercontent.com/127085765/235945975-49bbeff0-2f15-4f4e-808c-658a77b8770d.png">
     
     Sus propiedades (frecuencia de muestreo, número de canales, etc) se muestran en la siguiente tabla:
     &nbsp;&nbsp;<img width="235" alt="image" src="https://user-images.githubusercontent.com/127085765/235946611-8e2ec554-ecfa-4cde-88f5-485de8454f81.png">
     
     Este fichero de audio se encuentra con el resto de ficheros de esta práctica con el nombre `Audio_a`.
     
     	NOTA: es más que probable que tenga que usar Python, Octave/MATLAB u otro programa semejante para
	hacerlo. Se valorará la utilización de la biblioteca matplotlib de Python.
     
     Para la realización de los gráficos, hemos usado Matlab. El código implementado es el siguiente:
     &nbsp;&nbsp;<img width="331" alt="image" src="https://user-images.githubusercontent.com/127085765/235957124-a3cb7de4-6058-442f-a8ae-1b2906a8a257.png">

     Las gráficas obtenidas se muestran a continuación:
     &nbsp;&nbsp;<img width="957" alt="image" src="https://user-images.githubusercontent.com/127085765/235951287-48ac8110-30d5-47e2-b5cf-1c2587fd977c.png">
     
     
   * Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la
     autocorrelación. Inserte a continuación el código correspondiente.
     
     El fichero `pitch_analyzer.cpp` tiene una función llamada `compute_pitch`, que se encarga de encontrar
     el mejor candidato para el periodo de pitch. A continuación se muestra el código implementado:
     
     ```c
     if (x.size() != frameLen)
     	return -1.0F;
	
     for (unsigned int i=0; i<x.size(); ++i){
     	x[i] *= window[i];
     }
     
     vector<float> r(npitch_max);
     autocorrelation(x, r);
     vector<float>::const_iterator iR = r.begin(), iRMax = iR;
     
     for (iR = iRMax = r.begin() + npitch_min; iR <= r.begin() + npitch_max; iR++) {
     	if (*iR > *iRMax) {
				iRMax = iR;
			}
     }

   * Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.
    
     Mediante la modificación de `docopt`, se han modificado los argumentos que se pueden pasar cuando se 
     llama a `get_pitch` o a `run_get_pitch`. De este modo, para facilitar la determinación de los umbrales, 
     se pueden pasar todos como argumentos. En la función `unvoiced`, se comparan las características (potencia,
     r1norm, rmaxnorm y zcr) del segmento de señal analizado con los umbrales establecidos por defecto o 
     introducidos por el usuario al ejecutar el programa. A continuación se muestra el código implementado para 
     determinar el criterio de decisión de sordo/sonoro:
     ```
     if( (pot > this->u_pot || r1norm > this->u_r1) && rmaxnorm > this->u_rmax && zcr < u_zcr){
     	return false;
	 }else{
	 	return true;
	 }
   
   * Puede serle útil seguir las instrucciones contenidas en el documento adjunto `código.pdf`.

- Una vez completados los puntos anteriores, dispondrá de una primera versión del estimador de pitch. El 
  resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

  * Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un
    segmento es sonoro o sordo. 
	
	  - Inserte una gráfica con la estimación de pitch incorporada a `wavesurfer` y, junto a ella, los 
	    principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal
	    (r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la
	    autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).
	    
	    Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.
	    
	    Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que
	    en esta práctica es de 15 ms. 
	    	--> Se ha cambiado la configuración para que la separación entre muestras sea de 0.015 s.
	    
	    Para la obtención de los valores de los candidatos para determinar la sonoridad de la voz, hemos 
	    establecido que se muestren por pantalla los valores de potencia, r1norm, rmaxnorm y zcr obtenidos
	    para cada trama en el orden correspondiente. Ejecutamos `get_pitch` usando el fichero de audio de 
	    prueba como input:
	    
	    &nbsp;&nbsp;<img width="716" alt="image" src="https://user-images.githubusercontent.com/127085765/235986226-53f2029f-5654-4afb-8dd0-22aca8ba1a8e.png">
	    
	    A continuación, guardamos los datos en distintos ficheros:
	    
	    &nbsp;&nbsp;<img width="735" alt="image" src="https://user-images.githubusercontent.com/127085765/235987087-75be7284-7b92-4077-8c09-d07996210427.png">
	    
	    Abrimos `wavesurfer` y mostramos los datos gráficamente:
	    
		1. Señal Temporal: `prueba.wav`
		1. Contorno de pitch de la señal (Hz): calculado por `wavesurfer`
		1. Potencia de la señal (dB): `prueba.pot`
		1. r1norm de la señal: `prueba.r1`
		1. rmaxnorm de la señal: `prueba.rmax`
		1. zcr de la señal: `prueba.zcr`
	    	
	    &nbsp;&nbsp;<img width="958" alt="image" src="https://user-images.githubusercontent.com/127085765/236298857-36028001-f571-4f87-8f2b-48ef01859db6.png">

		
		3. Vemos que cuando la señal es sonora, el nivel de potencia es superior a -30 o -40 dB aproximadamente.
		   Por este motivo, decidimos que el valor por defecto de u_pot sea -30dB.
		4. Viendo los valores que toma la autocorrelación normalizada, consideramos que un umbral adecuado podría ser 0.6. 
		   Decidimos poner este valor como valor por defecto, de modo que si se supera, se considera que la señal es sonora.
		5. Observamos que el valor de la autocorrelación en el máximo secundario es aproximadamente superior a 0.4 cuando
		   la trama es sonora. Ponemos este valor por defecto.
		6. El valor de tasa de cruces por cero es prácticamente nulo excepto en algunos tramas cuyo valor se dispara por 
		   encima de 2000 aproximadamente. Por este motivo, hemos decidido poner como umbral por defectop el valor 2200.


      - Use el estimador de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare
	    su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica
		ilustrativa del resultado de ambos estimadores.
     
		Aunque puede usar el propio Wavesurfer para obtener la representación, se valorará
	 	el uso de alternativas de mayor calidad (particularmente Python).
		
		Usamos `wavesurfer` para comparar el contorno de pitch obtenido para la mejor versión de nuestro sistema 
		con el contorno de pitch calculado por el programa wavesurfer. Se han modificado las duraciones de ventana
		y de trama para que coincidan con los usados en nuestro sistema (duración ventana = 30 ms y se cambia de 
		trama cada 15 ms). El resultado obtenido es:
		
			1. Señal temporal: `prueba.wav`
			2. Contorno de pitch obtenido por `wavesurfer`
			3. Contorno de pitch obtenido por nuestro sistema: `prueba.f0`
		 
		&nbsp;&nbsp;<img width="956" alt="image" src="https://user-images.githubusercontent.com/127085765/236001701-5f6a6e2f-acaf-49bc-acc2-27714aba4340.png">

  
  * Optimice los parámetros de su sistema de estimación de pitch e inserte una tabla con las tasas de error
    y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
	`pitch_db/train`..
	
	En la siguiente imagen se observan los valores de los parámetros con los que hemos obtenido mejores resultados:
	
	&nbsp;&nbsp;<img width="502" alt="image" src="https://user-images.githubusercontent.com/127085765/236276248-92485648-aaac-4b38-a0aa-22d6de32b597.png">
	
	Los parámetros son:
	
	1. u_pot = -30
	2. u_r1 = 0.6
	3. u_rmax = 0.4
	4. u_zcr = 2200
	5. c_limit = 0.007
	
	A continuación se muestra la tabla del resumen con las tasas de error y el *score* TOTAL obtenido:
	
	&nbsp;&nbsp;<img width="384" alt="image" src="https://user-images.githubusercontent.com/127085765/236276839-b29fbf91-50f9-4811-87ca-21581ef3ef49.png">



Ejercicios de ampliación
------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  estimador a los argumentos de la línea de comandos.
  
  Esta técnica le resultará especialmente útil para optimizar los parámetros del estimador. Recuerde que
  una parte importante de la evaluación recaerá en el resultado obtenido en la estimación de pitch en la
  base de datos.

  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.
    
    Como ya se ha indicado anteriormente, se ha modificado el fichero `get_pitch.cpp` para incorporar los 
    parámetros del estimador a los argumentos de la línea de comandos. En la siguiente captura de pantalla 
    se muestra el mensaje de ayuda del programa:
    
    &nbsp;&nbsp;<img width="719" alt="image" src="https://user-images.githubusercontent.com/127085765/236281169-67387dbe-faa7-4e66-b2ac-40ca5459c041.png">
    
    
    Se usa el fichero `prueba.wav` para mostrar un ejemplo de utilización:
    
    &nbsp;&nbsp;<img width="725" alt="image" src="https://user-images.githubusercontent.com/127085765/236281754-a17dac3d-68c5-40b1-8f4e-92a93d6e858f.png">
    
    
    Para el uso de estos parámetros se ha modificado el fichero `run_get_pitch.sh`:
    
    &nbsp;&nbsp;<img width="316" alt="image" src="https://user-images.githubusercontent.com/127085765/236282089-99c16137-666e-4f5e-aad9-e9e7cd6c1c3d.png">




- Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de estimación
  de pitch.

  Entre las posibles mejoras, puede escoger una o más de las siguientes:

  * Técnicas de preprocesado: filtrado paso bajo, diezmado, *center clipping*, etc.
  * Técnicas de postprocesado: filtro de mediana, *dynamic time warping*, etc.
  * Métodos alternativos a la autocorrelación: procesado cepstral, *average magnitude difference function*
    (AMDF), etc.
  * Optimización **demostrable** de los parámetros que gobiernan el estimador, en concreto, de los que
    gobiernan la decisión sonoro/sordo.
  * Cualquier otra técnica que se le pueda ocurrir o encuentre en la literatura.

  Encontrará más información acerca de estas técnicas en las [Transparencias del Curso](https://atenea.upc.edu/pluginfile.php/2908770/mod_resource/content/3/2b_PS%20Techniques.pdf)
  y en [Spoken Language Processing](https://discovery.upc.edu/iii/encore/record/C__Rb1233593?lang=cat).
  También encontrará más información en los anexos del enunciado de esta práctica.

  Incluya, a continuación, una explicación de las técnicas incorporadas al estimador. Se valorará la
  inclusión de gráficas, tablas, código o cualquier otra cosa que ayude a comprender el trabajo realizado.

  También se valorará la realización de un estudio de los parámetros involucrados. Por ejemplo, si se opta
  por implementar el filtro de mediana, se valorará el análisis de los resultados obtenidos en función de
  la longitud del filtro.
  
  A continuación se muestran las mejoras implementadas:
  
  1. TÉCNICA DE PREPROCESADO: ***Center Clipping***
  
  	- Información: Consiste en anular los valores pequeños de x[n]. Su objetivo es aumentar la intensidad de 
	los armónicos de orden elevado y aumentar la robustez frente al ruido. Existen dos variantes:
	
	 &nbsp;&nbsp;<img width="659" alt="image" src="https://user-images.githubusercontent.com/127085765/236289507-a63bf702-fdea-4248-8d43-2b1cbf8bc95d.png">

	- **Con offset**:
	Los segmentos de amplitud mayor que el umbral (en valor absoluto) se desplazan hacia el cero, de modo que 
	la función de transformación es contínua. El código implementado es:
			
			for (unsigned int i=0; i<x.size(); ++i){
				if(x[i]>c_limit){
					x[i]=x[i]-c_limit;
				}else if(x[i]<-c_limit){
					x[i]=x[i]+c_limit;
				}else{
					x[i]=0;
				}
			}
			
	
	- **Sin offset**:
	Solo se modifican los valores de x[n] cuya amplitud es inferior al umbral c_limit, que se ponen a 0. Por lo tanto, 
	la función de transformación es discontinua. El código implementado es:
			
			for (unsigned int i=0; i<x.size(); ++i){
				if(abs(x[i])<c_limit){
					x[i]=0;
				}
			}
			
	A continuación se puede ver como quedaría la señal después del preprocesado:
	
	1. Señal original:

	&nbsp;&nbsp;<img width="594" alt="image" src="https://user-images.githubusercontent.com/127085765/236291915-bc7b5618-da93-4c00-b159-c0b7b70c4fe3.png">
	
	1. Después de aplicar center clipping con offset:
	
	&nbsp;&nbsp;<img width="581" alt="image" src="https://user-images.githubusercontent.com/127085765/236292212-5616d568-660b-43c2-972f-b0a8660146de.png">
	
	1. Después de aplicar center clipping sin offset:
	
	&nbsp;&nbsp;<img width="566" alt="image" src="https://user-images.githubusercontent.com/127085765/236292346-68dc319a-c589-4128-8fb7-92c247e68d70.png">



	Hemos ejecutado `run_get_pitch.sh` con las dos opciones y hemos obtenido mejor resultado usando *center clipping sin offset*.
	En nuestro caso, lo hemos implementado en el fichero `get_pitch.cpp`, de modo que se ha aplicado la técnica a nivel global de la 
	señal.
	
  1. TÉCNICA DE POSTPROCESADO: ***Filtro de Mediana***
  	- Información: El filtro de mediana es un filtro no lineal que se basa en calcular el valor mediano en una ventana centrada en 
	cada instante de tiempo. En nuestro caso hemos decidido implementarlo con un filtro de longitud 3. A continuación se muestra el 
	código implementado:
	
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
  	
	Los pasos seguidos para la implementación son:
	
	- Definimos el tamaño de la ventana usada: 3
		
	- Creación de dos vectores:
		vect: en cada iteración se guardan los tres valores de f0 qu se encuentran debajo
		del filtro centrado en la posición i-ésima.
		f0_ini: se inicializa con los valores de los dos primeros elementos de f0, ya que
		el filtro empieza centrado en la posición i=0. Como no hay muestra en i=-1, la 
		primera iteración la realizamos considerando que en i=-1 el valor es 0.
			
	- Se ordenan ascendentemente los tres valores de f0_ini.
		
	- Definimos un vector f0_filtered para ir guardando los valores resultantes de aplicar 
	el filtro de mediana. Al ser un filtro no recursivo, no podemos modificar f0, ya que
	la salida del filtro tiene que ser calculada usando únicamente los valores de entrada
	(no realimentación). 
		
	- El valor del primer elemento obtenido al aplicar el filtro de mediana es el valor que
	se encuentra en el índice central de f0_ini.
		
	- Se crea un bucle de tipo for() para iterar a lo largo de f0 (contiene los valores de pitch 
	obtenidos). Empieza en la posición i = 0 (filtro centrado en i=1), ya que ya hemos calculado 
	la salida para el caso en el que el filtro está centrado en la primera muestra. Termina en 
	i = # de elementos de f0 -2 (filtro centrado en # elementos de f0 -1).
		
	- Se crea otro bucle de tipo for() para asignar los valores correspondientes a vect.
		
	- Se ordenan ascendentemente los tres valores que contiene vect.
		
	- Se asigna al elemento i-ésimo de f0_filtered el valor mediano.
		
	- Se repite lo mismo que se ha hecho para el caso de filtro centrado en el primer elemento para
	el caso de filtro centrado en el último elemento
	
	A continuación se muestra una representación de su funcionamiento:
	
	Es importante destacar que es un filtro NO recursivo, motivo por el cual su salida solo puede depender 
	de la entrada y no hay realimentación. Este filtro es útil para eliminar los errores groseros.
  
  1. OPTIMIZACIÓN DE PARÁMETROS:
  	- 
	   

Evaluación *ciega* del estimador
-------------------------------

Antes de realizar el *pull request* debe asegurarse de que su repositorio contiene los ficheros necesarios
para compilar los programas correctamente ejecutando `make release`.

Con los ejecutables construidos de esta manera, los profesores de la asignatura procederán a evaluar el
estimador con la parte de test de la base de datos (desconocida para los alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.
