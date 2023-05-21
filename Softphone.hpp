#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "SSPAccount.hpp"
#include "SSPCall.hpp"
#include "SoftphoneArguments.hpp"

class Softphone
{
public:
    Softphone(const SoftphoneArguments & args,
    std::function<void(const pj::CallInfo&, pj::Call *, const int)> onIncomingCall,
    std::function<void(const pj::OnCallStateParam &, const pj::CallInfo &, const int)> onCallState,
    std::function<void(const pj::OnRegStateParam &, const pj::AccountInfo &, const int)> onRegState):
        _softphoneId(args.id),
        _account(std::to_string(args.id), args.domain, args.secret,
            std::bind(&Softphone::onRegState, this, std::placeholders::_1),
            std::bind(&Softphone::onCallState, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&Softphone::onIncomingCall, this, std::placeholders::_1, std::placeholders::_2)),
        _answeredIncomingCall(false),
        _declinedIncomingCall(false),
        _active(true),
        _onIncomingCall(onIncomingCall),
        _onCallState(onCallState),
        _onRegState(onRegState)
    {
        _uri = SIP + std::to_string(args.id) + SEPARATOR + args.domain;

        _account.apply();
    }

    ~Softphone()
    {
        _active = false;
    }

    void onIncomingCall(const pj::CallInfo& callInfo, pj::Call * incomingCall)
    {
        _onIncomingCall(callInfo, incomingCall, _softphoneId);
    }

    void onCallState(const pj::OnCallStateParam &prm, const pj::CallInfo &ci)
    {
        _onCallState(prm, ci, _softphoneId);
    }

    void onRegState(const pj::OnRegStateParam &prm)
    {
        pj::AccountInfo ai = _account.getInfo(); //move it into sspAccount
        _onRegState(prm, ai, _softphoneId);
    }

    void call(const Softphone &sp)
    {
        call(sp.getUri());
    }

    void call(const std::string & destUri)
    {
        _account.call(destUri);
    }

    void answer(const pj::CallOpParam& callOpParam)
    {
        _account.answer(callOpParam);
    }

    void hangup()
    {
        _account.hangup();
    }

    void setAnsweredIncomingCall(bool answer)
    {
        _answeredIncomingCall =  answer;
    }

    void setDeclinedIncomingCall(bool decline)
    {
        _declinedIncomingCall = decline;
    }

    bool getAnsweredIncomingCall()
    {
        return _answeredIncomingCall;
    }

    bool getDeclinedIncomingCall()
    {
        return _declinedIncomingCall;
    }

    void unregister()
    {
        _account.setRegistration(false);
    }

    int getId() const
    {
        return _softphoneId;
    }

    std::string getUri() const
    {
        return _uri;
    }

    bool isRegistered()
    {
        return _account.getInfo().regIsActive;
    }

    void setCall(pj::Call* newCall)
    {
        _account.setCall(newCall);
    }

    bool activeCall()
    {
        return _account.activeCall();
    }

    int getCallID()
    {
        return _account.getCallID();
    }

private:
    static constexpr auto SIP = "sip:";
    static constexpr auto SEPARATOR = "@";
    std::string _uri;
    const int _softphoneId;
    SSPAccount _account;
    bool _answeredIncomingCall;
    bool _declinedIncomingCall;
    bool _active;

    std::function<void(const pj::CallInfo&, pj::Call *, const int)> _onIncomingCall;
    std::function<void(const pj::OnCallStateParam &, const pj::CallInfo &, const int)> _onCallState;
    std::function<void(const pj::OnRegStateParam &, const pj::AccountInfo &, const int)> _onRegState;
};
