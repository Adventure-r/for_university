package main

import (
	"fmt"
	"math"
	"math/rand"
	"time"
)

// ln(sin(x)) - 1/x^2
func f(x float64) float64 {
	res := math.Sin(x)
	if res <= 0 {
		return 0
	}
	return math.Log(res) - 1.0/(x*x)
}

func monteCarloIntegral(a, b float64, n, iter int) (float64, float64) {
	rand.Seed(time.Now().UnixNano())

	allSum := 0.0
	sums := make([]float64, iter)
	for k := 0; k < iter; k++ {
		sum := 0.0

		for i := 0; i < n; i++ {
			x := a + (b-a)*rand.Float64()
			sum += f(x)
		}
		allSum += sum
		sums = append(sums, sum)
	}

	doubleSum := 0.0
	for _, i := range sums {
		s := i / float64(n)
		doubleSum += s * s
	}

	S := allSum / float64(iter*n)
	S2 := doubleSum / float64(iter)

	innacuracy := math.Sqrt(math.Abs(S2 - S*S))

	integral := S * (b - a)
	return integral, innacuracy
}

func main() {
	a := 0.32
	b := 1.52

	n := 200000
	iter := 5

	result, innacuracy := monteCarloIntegral(a, b, n, iter)
	fmt.Printf("Значение интеграла: %.6f\nНеточность: %.6f", result, innacuracy)
}
