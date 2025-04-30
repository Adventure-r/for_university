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
	order [3]string
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
	for _, order := range m.order {
		fmt.Print(order, "\t")
	}
	fmt.Println()
}

func (m exMatrix) findMax(rowStart, colStart, rowEnd, colEnd int) (rowTotal, colTotal int, maxTotal float64) {
	max := math.Abs(m.lines[rowStart].x[colStart])
	rowTotal = rowStart
	colTotal = colStart
	maxTotal = max

	// Пробежка по каждой строке
	for row := rowStart; row < rowEnd; row++ {
		// Пробежка по каждому столбцу
		for col := colStart; col < colEnd; col++ {
			// Поиск максимального по модулю числа для строки
			abs_i := math.Abs(m.lines[row].x[col])
			if abs_i > maxTotal {
				maxTotal = abs_i
				rowTotal = row
				colTotal = col
			}
		}
	}

	return
}

func (m *exMatrix) swap(rowFounded, colFounded, rowNeed, colNeed int) {
	// Меняются местами строчки
	row_buf := m.lines[rowNeed]
	m.lines[rowNeed] = m.lines[rowFounded]
	m.lines[rowFounded] = row_buf

	// Меняются местами переменные столбцов
	order_buf := m.order[colNeed]
	m.order[colNeed] = m.order[colFounded]
	m.order[colFounded] = order_buf

	// Меняются местами сами столбцы
	for i := range m.lines {
		x_buf := m.lines[i].x[colNeed]
		m.lines[i].x[colNeed] = m.lines[i].x[colFounded]
		m.lines[i].x[colFounded] = x_buf
	}
}

// Нужна для создания единицы (Предполагается, что value == одному из значений x)
func (l *line) divideTo1(value float64) {
	for i := 0; i < 3; i++ {
		if l.x[i] == 0. {
			continue
		}
		l.x[i] /= value
	}
	l.y /= value
}

// Нужна для создания нулей под rowStart в колонке colStart
func (m *exMatrix) differenceTo0(rowStart, colStart, rowEnd, colEnd int) {
	if rowStart == rowEnd {
		fmt.Println("differenceTo0: rowStart == rowEnd !!!")
	} else if rowStart < rowEnd {
		// Ход сверху вниз
		for row := rowStart + 1; row < rowEnd; row++ {
			value := m.lines[row].x[colStart] / m.lines[rowStart].x[colStart]
			for col := colStart; col < colEnd; col++ {
				m.lines[row].x[col] -= value * m.lines[rowStart].x[col]
			}
			m.lines[row].y -= value * m.lines[rowStart].y
		}
	} else if rowStart > rowEnd {
		// Ход снизу вверх
		for row := rowStart - 1; row > rowEnd; row-- {
			value := m.lines[row].x[colStart] / m.lines[rowStart].x[colStart]
			for col := colStart; col > colEnd; col-- {
				m.lines[row].x[col] -= value * m.lines[rowStart].x[col]
			}
			m.lines[row].y -= value * m.lines[rowStart].y
		}
	}
}

func (m *exMatrix) solveMatrix() (x, y, z float64) {
	colStart := 0
	rowEnd, colEnd := 3, 3

	// Прямой ход (приведение к верхнетреугольному виду)
	for rowStart := 0; rowStart < rowEnd; rowStart++ {
		row, col, value := m.findMax(rowStart, colStart, rowEnd, colEnd)
		fmt.Println("\nМаксимальный элемент расположен на: ", row, col, " Равен: ", value)
		m.swap(rowStart, colStart, row, col)
		m.printMatrix()
		m.lines[rowStart].divideTo1(value)
		fmt.Println("DivTo1")
		m.differenceTo0(rowStart, colStart, rowEnd, colEnd)
		fmt.Println("DiffTO0")
		colStart++
		m.printMatrix()
	}

	// Обратный ход (приведение к диагональному виду)
	for rowStart := rowEnd - 1; rowStart >= 0; rowStart-- {
		colStart = rowStart
		m.lines[rowStart].divideTo1(m.lines[rowStart].x[colStart])
		m.differenceTo0(rowStart, colStart, -1, -1)
		fmt.Println("Обратный ход:")
		m.printMatrix()
	}

	// Возвращаем решения
	solutions := make(map[string]float64)
	for i := 0; i < 3; i++ {
		solutions[m.order[i]] = m.lines[i].y
	}

	x = solutions["x"]
	y = solutions["y"]
	z = solutions["z"]

	return
}

func main() {
	// Заполнение матрицы
	var m exMatrix
	m.lines[0].fillM(3, 0, -1, 7)
	m.lines[1].fillM(2, -5, 1, -2)
	m.lines[2].fillM(2, 2, 5, 1)
	m.order[0] = "x"
	m.order[1] = "y"
	m.order[2] = "z"

	fmt.Println("Запуск!")
	m.printMatrix()

	x, y, z := m.solveMatrix()
	fmt.Println("\nРешения:")
	fmt.Printf("x = %.3f\n", x)
	fmt.Printf("y = %.3f\n", y)
	fmt.Printf("z = %.3f\n", z)
}
