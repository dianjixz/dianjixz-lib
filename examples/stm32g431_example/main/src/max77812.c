#include "max77812.h"

static void max77812_select(max77812_pmic_t pmic)
{
    switch (pmic) {
        case MAX77812_PMIC1:
            PMIC1_CS_ENABLE();  // PMIC1 CS
            break;
        case MAX77812_PMIC2:
            PMIC2_CS_ENABLE();  // PMIC2 CS
            break;
        default:
            break;
    }
}

static void max77812_deselect(max77812_pmic_t pmic)
{
    switch (pmic) {
        case MAX77812_PMIC1:
            PMIC1_CS_DISABLE();  // PMIC1 CS
            break;
        case MAX77812_PMIC2:
            PMIC2_CS_DISABLE();  // PMIC2 CS
            break;
        default:
            break;
    }
}

/**
 * @brief Write single byte to MAX77812 register
 *
 * @param reg Register address
 * @param data Data byte to write
 * @return HAL status
 */
HAL_StatusTypeDef max77812_write_reg(max77812_pmic_t pmic, uint8_t reg, uint8_t data)
{
    uint8_t tx_buf[4] = {
        MAX77812_SPI_HEADER(MAX77812_RW_WRITE, MAX77812_SINGLE_OP),  // R/W = 1 (write), S/M = 0 (single)
        reg,                                                         // Address
        0x01,                                                        // Packet length
        data                                                         // Data
    };

    max77812_select(pmic);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, tx_buf, 4, MAX77812_SPI_TIMEOUT);
    max77812_deselect(pmic);

    return status;
}

/**
 * @brief Read single byte from MAX77812 register
 *
 * @param reg Register address
 * @param data Pointer to store read byte
 * @return HAL status
 */
HAL_StatusTypeDef max77812_read_reg(max77812_pmic_t pmic, uint8_t reg, uint8_t *data)
{
    uint8_t tx_buf[3] = {
        MAX77812_SPI_HEADER(MAX77812_RW_READ, MAX77812_SINGLE_OP),  // R/W = 0 (read), S/M = 0 (single)
        reg,                                                        // Address
        0x01,                                                       // Packet length
    };
    max77812_select(pmic);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, tx_buf, 3, MAX77812_SPI_TIMEOUT);
    if (status == HAL_OK) {
        status = HAL_SPI_Receive(&hspi1, (uint8_t *)data, 1, MAX77812_SPI_TIMEOUT);
    }
    max77812_deselect(pmic);
    return status;
}

/**
 * @brief Write multiple bytes to consecutive MAX77812 registers
 *
 * @param reg Start register address
 * @param data Pointer to data buffer
 * @param size Number of bytes to write
 * @return HAL status
 */
HAL_StatusTypeDef max77812_mult_write_reg(max77812_pmic_t pmic, uint8_t reg, const uint8_t *data, uint8_t size)
{
    if (data == NULL || size == 0) return HAL_ERROR;

    uint8_t header[3] = {
        MAX77812_SPI_HEADER(MAX77812_RW_WRITE, MAX77812_MULTIPLE_OP), reg,
        size - 1  // PACKET_LENGTH
    };

    max77812_select(pmic);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, header, sizeof(header), MAX77812_SPI_TIMEOUT);
    if (status == HAL_OK) {
        status = HAL_SPI_Transmit(&hspi1, (uint8_t *)data, size, MAX77812_SPI_TIMEOUT);
    }
    max77812_deselect(pmic);
    return status;
}

/**
 * @brief Read multiple bytes from consecutive MAX77812 registers
 *
 * @param reg Start register address
 * @param data Pointer to buffer for read data
 * @param size Number of bytes to read
 * @return HAL status
 */
HAL_StatusTypeDef max77812_mult_read_reg(max77812_pmic_t pmic, uint8_t reg, uint8_t *data, uint8_t size)
{
    if (data == NULL || size == 0) return HAL_ERROR;

    uint8_t header[3] = {
        MAX77812_SPI_HEADER(MAX77812_RW_READ, MAX77812_MULTIPLE_OP), reg,
        size - 1  // PACKET_LENGTH
    };

    max77812_select(pmic);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, header, sizeof(header), MAX77812_SPI_TIMEOUT);
    if (status == HAL_OK) {
        status = HAL_SPI_Receive(&hspi1, data, size, MAX77812_SPI_TIMEOUT);
    }
    max77812_deselect(pmic);

    return status;
}
