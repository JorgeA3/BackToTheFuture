import sympy as sp
import numpy as np

# Definir la variable simbólica para la ecuación de regresión
x = sp.Symbol('x')

# Nueva ecuación de regresión polinomial de grado 2 proporcionada
# Coeficientes: c = 140.2359, a = -0.1164, b = 0.00001
poly = 140.2359 + (-0.1164) * x + (0.00001) * x**2

# Convertir la expresión simbólica en una función de NumPy para un cálculo rápido
f = sp.lambdify(x, poly, "numpy")

# El tamaño de la tabla es 2^12 = 4096 (para un ADC de 12 bits)
lut_size = 2**12

# Calcular todos los valores de la tabla de búsqueda de manera eficiente
# Usamos np.arange para generar todos los valores de entrada del ADC (0 a 4095)
values = f(np.arange(lut_size))
values = np.clip(values, 0, 100)

# Escribir en el archivo de cabecera C
with open("lookuptable.h", "w") as file:
    file.write("#ifndef LOOKUPTABLE_H\n")
    file.write("#define LOOKUPTABLE_H\n\n")
    file.write("#define LUT_SIZE {}\n\n".format(lut_size))
    file.write("static const uint32_t percentage_values[LUT_SIZE] = {\n")
    
    # Formatear el array con 16 valores por línea para una mejor lectura
    for i in range(0, lut_size, 16):
        row = ", ".join(str(int(v)) for v in values[i:i+16])
        file.write("  {},\n".format(row))
        
    file.write("};\n\n")
    file.write("#endif // LOOKUPTABLE_H\n")

print("Archivo lookuptable.h generado exitosamente.")