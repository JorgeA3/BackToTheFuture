import pandas as pd

# Cargar el archivo CSV
df = pd.read_csv('datos_humedad.csv')

# Transformar de formato "ancho" a "largo"
# 'Porcentaje de humedad' será la columna fija
# Las columnas 'Voltaje...' se convertirán en filas
df_long = df.melt(id_vars=['Porcentaje de humedad'],
                  var_name='Prueba',
                  value_name='Voltaje')

# Eliminar las filas con valores nulos (NaN)
# Esto es necesario para la regresión, ya que los valores faltantes pueden causar errores
df_long.dropna(inplace=True)

# Convertir el tipo de datos a numérico para asegurar que la regresión funcione correctamente
df_long['Porcentaje de humedad'] = pd.to_numeric(df_long['Porcentaje de humedad'])
df_long['Voltaje'] = pd.to_numeric(df_long['Voltaje'])

print(df_long.head())
print("\nDimensiones de los datos limpios:", df_long.shape)


import numpy as np
from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression

# Definir las variables X (voltaje) e y (humedad)
# Queremos predecir el % de humedad a partir del voltaje, por lo tanto:
X = df_long[['Voltaje']]
y = df_long['Porcentaje de humedad']

# Crear un objeto para transformar las características a un grado polinomial
poly_features = PolynomialFeatures(degree=2, include_bias=False)
X_poly = poly_features.fit_transform(X)

# Entrenar el modelo de regresión lineal sobre las características polinomiales
model = LinearRegression()
model.fit(X_poly, y)

# Obtener los coeficientes del modelo
# El coeficiente del primer término es el que acompaña a 'Voltaje' y el del segundo a 'Voltaje^2'
a, b = model.coef_
c = model.intercept_

print(f"\nCoeficientes del modelo:")
print(f"a = {a:.4f} (coeficiente de Voltaje)")
print(f"b = {b:.4f} (coeficiente de Voltaje^2)")
print(f"c = {c:.4f} (intercepto)")
print(f"\nEcuación del modelo: Humedad = {c:.4f} + ({a:.4f}) * Voltaje + ({b:.4f}) * Voltaje^2")

import matplotlib.pyplot as plt

# Generar puntos para la curva ajustada
X_plot = np.linspace(X.min(), X.max(), 100).reshape(-1, 1)
X_plot_poly = poly_features.transform(X_plot)
y_plot = model.predict(X_plot_poly)

# Crear la gráfica de dispersión y la curva ajustada
plt.figure(figsize=(10, 6))
plt.scatter(X, y, color='blue', label='Datos originales')
plt.plot(X_plot, y_plot, color='red', linewidth=2, label='Curva de Regresión (Polinomio de grado 2)')
plt.title('Regresión Polinomial de Humedad vs. Voltaje')
plt.xlabel('Voltaje (mV)')
plt.ylabel('Porcentaje de Humedad (%)')
plt.legend()
plt.grid(True)
plt.show()