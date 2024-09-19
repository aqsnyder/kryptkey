/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ssd1306.h"  // Include your SSD1306 driver
#include "ssd1306_fonts.h"  // Include the fonts you need

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define DEBOUNCE_DELAY 10  // 50ms debounce time

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

// Account data
const char* account_names[] = {"Amazon", "Gmail", "Spotify"};
const char* usernames[] = {"tuf50288@temple.edu", "", ""};
const char* passwords[] = {"#*FBNhf3478*@#hfhfh*#845h3h3GKJ$#3$58fha", "", ""};
int current_selection = 0;  // Tracks the currently selected item

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
void display_menu(void);
void navigate_menu(int direction);
void check_buttons(void);
void show_account_details(int index);
uint8_t debounce_button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Display the menu of account names */
void display_menu() {
    ssd1306_Fill(Black);  // Clear the screen

    for (int i = 0; i < sizeof(account_names) / sizeof(account_names[0]); i++) {
        ssd1306_SetCursor(2, i * 16);  // Set Y position for each line

        if (i == current_selection) {
            ssd1306_WriteString(account_names[i], Font_11x18, White);  // Highlight selected item
        } else {
            ssd1306_WriteString(account_names[i], Font_7x10, White);
        }
    }
    ssd1306_UpdateScreen();  // Send buffer to display
}

/* Handle menu navigation based on button input */
void navigate_menu(int direction) {
    int menu_size = sizeof(account_names) / sizeof(account_names[0]);
    current_selection += direction;
    if (current_selection < 0) {
        current_selection = menu_size - 1;  // Wrap to bottom
    } else if (current_selection >= menu_size) {
        current_selection = 0;  // Wrap to top
    }
    display_menu();  // Update the display
}

/* Display account details when selected */
void show_account_details(int index) {
    ssd1306_Fill(Black);  // Clear the screen

    int x_position = 2;   // Left-align text
    int max_chars_per_line = 21;  // 128 / 6 = 21 characters per line for Font_6x8

    // --- Display Username ---
    ssd1306_SetCursor(x_position, 8);  // First line for username
    if (strlen(usernames[index]) > max_chars_per_line) {
        // Split the username into two lines if it's too long
        char first_part[max_chars_per_line + 1];
        strncpy(first_part, usernames[index], max_chars_per_line);  // Copy the first part
        first_part[max_chars_per_line] = '\0';
        ssd1306_WriteString(first_part, Font_6x8, White);  // Write first part

        ssd1306_SetCursor(x_position, 16);  // Move to the next line for the rest
        ssd1306_WriteString(usernames[index] + max_chars_per_line, Font_6x8, White);
    } else {
        // Username fits on one line
        ssd1306_WriteString(usernames[index], Font_6x8, White);
    }

    // --- Display Password ---
    ssd1306_SetCursor(x_position, 32);  // Leave an empty line before the password
    if (strlen(passwords[index]) > max_chars_per_line) {
        // Split the password into two lines if it's too long
        char first_part[max_chars_per_line + 1];
        strncpy(first_part, passwords[index], max_chars_per_line);  // Copy the first part
        first_part[max_chars_per_line] = '\0';
        ssd1306_WriteString(first_part, Font_6x8, White);  // Write first part

        ssd1306_SetCursor(x_position, 40);  // Move to the next line for the rest
        ssd1306_WriteString(passwords[index] + max_chars_per_line, Font_6x8, White);
    } else {
        // Password fits on one line
        ssd1306_WriteString(passwords[index], Font_6x8, White);
    }

    ssd1306_UpdateScreen();  // Send buffer to display
}


/* Debounce button presses */
uint8_t debounce_button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET) {
        HAL_Delay(DEBOUNCE_DELAY);  // Wait for debounce time
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET) {
            return 1;  // Button is confirmed pressed
        }
    }
    return 0;  // Button is not pressed
}

/* Check the state of buttons and navigate the menu */
void check_buttons() {
    if (debounce_button(GPIOA, GPIO_PIN_4)) {
        navigate_menu(-1);  // Move up in the list
    } else if (debounce_button(GPIOA, GPIO_PIN_5)) {
        navigate_menu(1);   // Move down in the list
    } else if (debounce_button(GPIOA, GPIO_PIN_6)) {
        show_account_details(current_selection);  // Show details for the selected account
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */
  ssd1306_Init();  // Initialize the OLED display
  display_menu();  // Show the initial menu

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    check_buttons();  // Poll buttons for input
    HAL_Delay(200);   // Add delay to avoid button bouncing
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void) {
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure GPIO pins : PA4 PA5 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;  // Enable pull-down resistors to prevent floating pins
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}
