package main

import (
	"fmt"
	"math"
)

// ln(sin(x)) - 1/x^2
func f(x float64) float64 {
	sin := math.Sin(x)
	if sin <= 0 {
		return 0
	}
	return math.Log(sin) - 1.0/(x*x)
}

func simpson(a, b float64, n int) float64 {
	h := (b - a) / float64(n)
	sum := f(a) + f(b)

	for i := 1; i < n; i++ {
		x := a + float64(i)*h
		if i%2 == 0 {
			sum += 2 * f(x)
		} else {
			sum += 4 * f(x)
		}
	}

	return (h / 3) * sum
}

// Адаптивный метод Симпсона
func aS(a, b, eps float64, l int) (float64, int) {
	I1 := simpson(a, b, 2)
	I2 := simpson(a, b, 4)
	errorEstimate := math.Abs(I2-I1) / 15

	if errorEstimate < eps {
		return I2, l
	}

	// Разделяем интервал на две части и рекурсивно применяем метод
	mid := (a + b) / 2
	res1, l1 := aS(a, mid, eps/2, l+1)
	res2, l2 := aS(mid, b, eps/2, l+1)

	return res1 + res2, l1 + l2
}

func main() {
	a := 1.
	b := 3.
	eps := 0.00001

	result, iter := aS(a, b, eps, 0)
	fmt.Printf("Значение интеграла: %.6f\nКоличество итераций: %.v", result, iter)
}
