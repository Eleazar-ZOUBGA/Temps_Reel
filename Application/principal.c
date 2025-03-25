#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 128
uint32_t user_stack[STACK_SIZE]; // Pile stockée en RAM

// Initialisation des deux piles pour chacune des deux fonctions
uint32_t * stack1 = (uint32_t *) 0x20002000;
uint32_t * stack2 = (uint32_t *) 0x20003000;

// Déclaration des pointers de pile pour chacune des piles
uint32_t * psp1;
uint32_t * psp2;
uint32_t * current_psp;

volatile int task1_counter = 0;
volatile int task2_counter = 0;

void switch_to_unprivileged_mode(void){
	/*uint32_t ctrl = __get_CONTROL();
	__set_CONTROL(ctrl);
	
	uint32_t psp_value = __get_PSP();
	__set_PSP(psp_value); */
	
	//__set_PSP((uint32_t)&user_stack[STACK_SIZE]); // PSP est en haut de la pile utilisateur
	__set_CONTROL(0x02); // Bit 1 = 1 -> Utilisation de PSP, Bit 0 = 0 -> Mode Non-Privilégié
	__ISB(); // Instruction de synchronisation pour s'assurer que les changements ont bien été prises en compte
}

void SysTick_Handler(void){
	int i = 0;
}

void SysTick_Init(void){
	// Calcul du nombre de cycles toutes les ms
	SysTick->LOAD = (72000000 / 1000) - 1;
	SysTick->VAL = 0; // Initialisation du compteur
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk;
}

void function1(void){ while(1){task1_counter++;} }
void function2(void){ while(1){task2_counter++;} }

int main ( void )
{
	RCC->APB2ENR |= (0x01 << 2) | (0x01 << 3) | (0x01 << 4) ;
	
	GPIOA->CRL &= ~(0xF<<4) ;
	GPIOA->CRL |= (0x2<<4) ;
	
	SysTick_Init();
	
	switch_to_unprivileged_mode();
	
	//Initialisation de la pile pour la fonction 1 :
	psp1 = stack1;
	stack1 --; *stack1 = 0x01000000; // xPSR
	stack1 --; *stack1 = (uint32_t)function1; // PC	
	stack1 --; *stack1 = 0xFFFFFFFD; // LR	
	stack1 --; *stack1 = 0x00000000; // R12	
	stack1 --; *stack1 = 0x00000000; // R3	
	stack1 --; *stack1 = 0x00000000; // R2	
	stack1 --; *stack1 = 0x00000000; // R1	
	stack1 --; *stack1 = 0x00000000; // R0
	
	//Initialisation de la pile pour la fonction 2 :
	psp2 = stack2;
	stack2 --; *stack2 = 0x01000000; // xPSR
	stack2 --; *stack2 = (uint32_t)function2; // PC	
	stack2 --; *stack2 = 0xFFFFFFFD; // LR	
	stack2 --; *stack2 = 0x00000000; // R12	
	stack2 --; *stack2 = 0x00000000; // R3	
	stack2 --; *stack2 = 0x00000000; // R2	
	stack2 --; *stack2 = 0x00000000; // R1	
	stack2 --; *stack2 = 0x00000000; // R0
	
	while (1)
	{
	}
}
