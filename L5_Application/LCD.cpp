#include "tasks.hpp"
#include "examples/examples.hpp"
#include "io.hpp"
#include "uart2.hpp"


class dispLCDTask : public scheduler_task
{
    public:
        dispLCDTask(uint8_t priority) :
            scheduler_task("dispLCDTask", 2048, priority),
            my_uart_tx(Uart2::getInstance())
        {
            //my_uart_tx.init(9600);
            //my_uart_tx.init(115200);
        }

        bool init(){
            my_uart_tx.init(38400);
            return true;
        }

        bool run(void *p)
        {
            if (i) {
                char outchar[4] = {'r','p','m',':'};//Output rpm values -> read from queues
                char rpm[2] = {'5','5'};
                vTaskDelay(100);
                my_uart_tx.putline(outchar, 0);
                vTaskDelay(100);
                my_uart_tx.putline(rpm,0);
                vTaskDelay(100);
            }
            i=0;
            return true;
        }

        private:
            Uart2 &my_uart_tx;
            int i = 1;
};
