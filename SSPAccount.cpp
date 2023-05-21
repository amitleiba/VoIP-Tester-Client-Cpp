#include "SSPAccount.hpp"

#include "SSPCall.hpp"

SSPAccount::SSPAccount(const std::string &id, const std::string &domain, const std::string &secret,
                std::function<void(const pj::OnRegStateParam &)> onRegState,
                std::function<void(const pj::OnCallStateParam &, const pj::CallInfo &)> onCallState,
                std::function<void(const pj::CallInfo&, pj::Call *)> onIncomingCall):
    _onRegState(std::move(onRegState)),
    _onCallState(std::move(onCallState)),
    _onIncomingCall(std::move(onIncomingCall)),
    _call(nullptr)
{
    std::string uri = SIP + id + SEPARATOR + domain;
    _config.idUri = uri;
    _config.regConfig.registrarUri = SIP + domain;

    pj::AuthCredInfo aci(SCHEME, REALM, id, DATA_TYPE, secret);

    _config.sipConfig.authCreds.push_back(aci);
}

SSPAccount::~SSPAccount()
{
    // shutdown();
    std::cout << "*** Account " + std::to_string(getId()) +" is being deleted ***" << std::endl;
}

void SSPAccount::apply()
{
    try{
        create(_config);
    } catch (...) {
        std::cerr << "Adding account failed" << std::endl;
    }
}

void SSPAccount::onRegState(pj::OnRegStateParam &prm)
{
    _onRegState(prm);
}

void SSPAccount::onIncomingCall(pj::OnIncomingCallParam &iprm)
{
    pj::Call* incomingCall = new SSPCall(*this, std::bind(&SSPAccount::onCallState, this,
            std::placeholders::_1, std::placeholders::_2), iprm.callId);

    auto ci = incomingCall->getInfo();

    _onIncomingCall(ci, incomingCall);
}

void SSPAccount::onCallState(const pj::OnCallStateParam &prm,  const pj::CallInfo& ci)
{
    _onCallState(prm, ci);
    if(_call && _call->getInfo().id == ci.id && ci.state == PJSIP_INV_STATE_DISCONNECTED)
    {
        _call = nullptr;
    } 
}

void SSPAccount::call(const std::string & destUri)
{
    if(!_call)
    {
        _call = new SSPCall(*this, std::bind(&SSPAccount::onCallState, this,
                std::placeholders::_1, std::placeholders::_2));

    }
    if(getInfo().regIsActive && !_call->isActive())
    {
        pj::CallOpParam prm(true);
        prm.opt.audioCount = 1;
        prm.opt.videoCount = 0;
        _call->makeCall(destUri, prm);
    }
}

void SSPAccount::hangup()
{
    if(activeCall())
    {
        pj::CallOpParam opcode;
        opcode.statusCode = PJSIP_SC_DECLINE;
        _call->hangup(opcode);
    }
}

void SSPAccount::setCall(pj::Call* newCall)
{
    _call = newCall;
}

bool SSPAccount::activeCall()
{
    return _call && _call->isActive();
}

void SSPAccount::answer(const pj::CallOpParam& callOpParam)
{
    _call->answer(callOpParam);
}

int SSPAccount::getCallID()
{
    if(_call)
        return _call->getId();
    return -1;
}
