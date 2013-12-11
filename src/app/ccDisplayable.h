#ifndef CC_DISPLAYABLE_HEADER
#define CC_DISPLAYABLE_HEADER

#include <QObject>
#include <QString>

namespace CCF
{
    class Displayable: public QObject
    {
        public:
            virtual ~Displayable();

            const QString& GetName() const;
            int GetIdentifier() const;
            int GetSize() const;


        public slots:

        signals:

        protected:
            Displayable(const QString& _name);

            int identifier;
            QString name;


        protected slots:

        private:

    }; // class Displayable
} // namespace CCF
#endif // #ifndef CC_DISPLAYABLE_HEADER