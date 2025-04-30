package main

import (
	"fmt"
)

type Hero struct {
	name      string
	hp        int
	mp        int
	xp        int
	inventory map[int]int
}

type Item struct {
	name        string
	discription string
	id          int
}

func main() {
	fmt.Print("Привет, чмо. Ты лошара. Я знаю, можешь поплакать или уйти.(1 или 2)\nНомер действия... ")

}
