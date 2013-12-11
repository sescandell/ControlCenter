#ifndef CC_KHEPERA_COMMUNICATOR_HEADER
#define CC_KHEPERA_COMMUNICATOR_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QThread>
#include <QSemaphore>
#include <QByteArray>
#include <QString>

/*---------------------------------------------------------- Intern includes */
#include "ccProtocolProcessor.h"

namespace CCF
{
    class Robot;

    class Communicator: public QThread
	{
        Q_OBJECT

		public:
            enum Mode { COMMUNICATOR_MODE_MULTI_ROBOT, COMMUNICATOR_MODE_SINGLE_ROBOT };
            /// <summary>
            ///     Constructor
            ///</summary>
            Communicator(const QString& interfaceName, Mode _mode);

            /// <summary>
            ///     Destructor
            ///</summary>
            ~Communicator();

            bool AddRobot(const Robot* robot);
            bool Connect();
            bool Disconnect();
            QByteArray  GetData(const Robot* robot = 0);
            QList<QByteArray> GetStatistics(const Robot* robot = 0);
            const QString& GetInterfaceName() const;
            Mode GetMode() const;
            bool IsConnected();
            bool RemoveRobot(const Robot* robot);
            void SendData(QByteArray& data, const Robot* robot = 0);

        signals:



        protected:
            /*--------------------------------------------- Protected methods*/
            void run();

            /*------------------------------------------ Protected attributes*/
            QString            interfaceName;
            Mode               mode;
            ProtocolProcessor* protocolProcessor;
            QSemaphore         semReady;
            

        protected slots:
            
    
        private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/


	}; // class Communicator
} // namespace CCF

#endif // #ifndef CC_KHEPERA_COMMUNICATOR_HEADER