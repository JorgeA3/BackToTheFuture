float calculate_humidity(int adc_voltage_mv) {
    // Coeficientes del modelo de regresión polinomial de grado 2
    // Ecuación: Humedad = c + a * Voltaje + b * Voltaje^2
    const float a = -0.1691;
    const float b = 0.0000; // Valor muy pequeño, se puede redondear a 0
    const float c = 265.4184;

    // Convertir el voltaje de int a float para los cálculos
    float voltage_float = (float)adc_voltage_mv;
    
    // Calcular la humedad
    float humidity = c + (a * voltage_float) + (b * voltage_float * voltage_float);

    // Asegurarse de que el valor de humedad esté dentro del rango [0, 100]
    if (humidity > 100.0) {
        return 100.0;
    }
    if (humidity < 0.0) {
        return 0.0;
    }
    
    return humidity;
}
