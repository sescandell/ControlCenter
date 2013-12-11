#ifndef CC_TRACK_LAYER_HEADER
#define CC_TRACK_LAYER_HEADER

#include "ccLayer.h"
#include "ccDisplayable.h"

#include "../track/ccTrack.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>




namespace CCF
{
    class TrackLayer: public Layer
    {
        Q_OBJECT

        public:
            TrackLayer(const Displayable* _object, QWidget* parent=0);

            LayerType GetLayerType() const;

        protected:
            const Track* track() const;

            // Design variables
            void setupSpecificInterface();

            QLabel* laPointsCount;

        protected slots:            

    }; // class TrackLayer
} // namespace

#endif  // #ifndef CC_TRACK_LAYER_HEADER