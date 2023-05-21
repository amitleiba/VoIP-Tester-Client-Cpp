#include "SSPCall.hpp"

#include "SSPAccount.hpp"

SSPCall::SSPCall(pj::Account &acc,
                 std::function<void(const pj::OnCallStateParam &, const pj::CallInfo&)> onCallState,
                 int call_id):
    pj::Call(acc, call_id),
    _onCallState(std::move(onCallState))
{

}

SSPCall::~SSPCall()
{
    std::cout << "Call was deleted" << std::endl;
}

void SSPCall::onCallState(pj::OnCallStateParam &prm)
{
    pj::CallInfo ci = getInfo();
    _onCallState(prm, ci);
    if (ci.state == PJSIP_INV_STATE_DISCONNECTED)
    {
        delete this;
    }
}

void SSPCall::onCallMediaState(pj::OnCallMediaStateParam &prm)
{
    PJ_UNUSED_ARG(prm);
    pj::CallInfo ci = getInfo();
    // Iterate all the call medias
    for (unsigned i = 0; i < ci.media.size(); i++) {
        if (ci.media[i].type==PJMEDIA_TYPE_AUDIO && getMedia(i)) {
            pj::AudioMedia *aud_med = (pj::AudioMedia *)getMedia(i);

            // Connect the call audio media to sound device
            pj::AudDevManager& mgr = pj::Endpoint::instance().audDevManager();
            aud_med->startTransmit(mgr.getPlaybackDevMedia());
            mgr.getCaptureDevMedia().startTransmit(*aud_med);
        }
    }
}
