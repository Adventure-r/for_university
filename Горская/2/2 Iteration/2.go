package main

import (
	"fmt"
	"math"
)

type line struct {
	x [3]float64
	y float64
}

type exMatrix struct {
	lines [3]line
}

func (l *line) fillM(x1, x2, x3, y_init float64) {
	l.x[0] = x1
	l.x[1] = x2
	l.x[2] = x3
	l.y = y_init
}

func (m exMatrix) printMatrix() {
	fmt.Println("\nМатрица: ")

	for _, line := range m.lines {
		for _, x := range line.x {
			fmt.Printf("%.3f\t", x)
		}
		fmt.Printf("|\t%.3f\n", line.y)
	}
	fmt.Println()
}

type XesMap map[int]float64

func (xes XesMap) printXes() {
	fmt.Println("\nИксы: ")

	for i := 1; i <= 3; i++ {
		fmt.Printf("x%d = %.6f\n", i, xes[i])
	}

	fmt.Println()
}

// Проверка матрицы на пригодность для решения методом простых итераций
func (m exMatrix) firstCheck() (isOk bool) {
	isOk = true
	if math.Abs(m.lines[0].x[0]) <= (math.Abs(m.lines[0].x[1]) + math.Abs(m.lines[0].x[2])) {
		isOk = false
		return
	}
	if math.Abs(m.lines[1].x[1]) <= math.Abs(m.lines[1].x[0])+math.Abs(m.lines[1].x[2]) {
		isOk = false
		return
	}
	if math.Abs(m.lines[2].x[2]) <= math.Abs(m.lines[2].x[0])+math.Abs(m.lines[2].x[1]) {
		isOk = false
		return
	}
	return
}

func inacuracyCheck(epsi, xNow, xWas float64) (isOk bool) {
	return math.Abs(xNow-xWas)/math.Abs(xNow) <= epsi
}

func (m *exMatrix) solveByIterations(epsi float64) (xes XesMap, Error string) {
	//Проверка на первоначальное условие
	Error = ""
	//if !m.firstCheck() {
	//	Error = "Матрица не подходит для решения методом простых итераций!"
	//	return
	//}

	xes = XesMap{
		1: 0.,
		2: 0.,
		3: 0.,
	}

	//Нулевое приближение
	for i := 0; i < 3; i++ {
		xes[i+1] = m.lines[i].y / m.lines[i].x[i]
	}

	checks := map[int]bool{
		1: false,
		2: false,
		3: false,
	}
	prevXes := make(XesMap, 3)
	xes[1] = (m.lines[0].y + prevXes[2]*(-m.lines[0].x[1]) + prevXes[3]*(-m.lines[0].x[2])) / (m.lines[0].x[0])
	xes[2] = (m.lines[1].y + prevXes[1]*(-m.lines[1].x[0]) + prevXes[3]*(-m.lines[1].x[2])) / (m.lines[1].x[1])
	xes[3] = (m.lines[2].y + prevXes[2]*(-m.lines[2].x[1]) + prevXes[1]*(-m.lines[2].x[0])) / (m.lines[2].x[2])

	counter := 0
	for k := 1; ; k++ {
		for i := 1; i <= 3; i++ {
			prevXes[i] = xes[i]
		}
		xes[1] = (m.lines[0].y + prevXes[2]*(-m.lines[0].x[1]) + prevXes[3]*(-m.lines[0].x[2])) / (m.lines[0].x[0])
		xes[2] = (m.lines[1].y + prevXes[1]*(-m.lines[1].x[0]) + prevXes[3]*(-m.lines[1].x[2])) / (m.lines[1].x[1])
		xes[3] = (m.lines[2].y + prevXes[2]*(-m.lines[2].x[1]) + prevXes[1]*(-m.lines[2].x[0])) / (m.lines[2].x[2])

		// Проверяем только те x, которые еще не прошли проверку
		prevXes.printXes()
		xes.printXes()

		for i := 1; i <= 3; i++ {
			if !checks[i] && inacuracyCheck(epsi, xes[i], prevXes[i]) {
				checks[i] = true
				fmt.Print(i, " - икс с этим номером прошёл проверку\n", xes[i], "   |  ", prevXes[i])
				counter++
			}
		}
		fmt.Println("\ncounter = ", counter)
		fmt.Print("k = ", k, "\n-------------\n")
		if counter == 3 {
			break
		}

	}

	return
}

func main() {
	// Заполнение матрицы
	var m exMatrix
	m.lines[0].fillM(3, 0, -1, 7)
	m.lines[1].fillM(2, -5, 1, -2)
	m.lines[2].fillM(20, 2, 5, 1)

	epsi := 0.00001

	fmt.Println("Запуск!")
	m.printMatrix()

	xes, Error := m.solveByIterations(epsi)
	if Error == "" {
		fmt.Println("\nРешения:")
		xes.printXes()
	} else {
		fmt.Print(Error)
	}

	fmt.Print("\nlol\rbob\ndff\tfdf\trtrt")

}
