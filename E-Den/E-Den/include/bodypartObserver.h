// bodypartObserver.h
// an interface layer to supply information about bodypart creation and 
// by Franz Koehler 2009

#ifndef _E_DEN_BODYPARTOBSERVER_HEADER_
#define _E_DEN_BODYPARTOBSERVER_HEADER_

namespace EDen {
  class BodypartObserver;
  class BodypartInformation {
  public:
    BodypartInformation() {};
    virtual ~BodypartInformation() {};
  };
};

#include "globals.h"
#include <boost/thread/mutex.hpp>

namespace EDen {

  class BodypartObserver {
  private:
    bool updateBodypartInformation(Bodypart* bodypart, BodypartInformation* information);
  protected:  
    std::map<Bodypart*, BodypartInformation*> bodyparts;
    boost::mutex bodypartsMutex;

    BodypartInformation* getInformation(Bodypart* param_bodypart);

    virtual BodypartInformation* updateInformation(Bodypart* bodypart, BodypartInformation* information = 0);

    virtual bool bodypartAdded(Bodypart* bodypart, BodypartInformation* information) = 0;
    virtual bool bodypartRemoved(Bodypart* bodypart, BodypartInformation* information) = 0;

  public:
    BodypartObserver();
    ~BodypartObserver();

    bool addBodypart(Bodypart* param_bodypart);
    bool removeBodypart(Bodypart* param_bodypart);
  };
};

#endif