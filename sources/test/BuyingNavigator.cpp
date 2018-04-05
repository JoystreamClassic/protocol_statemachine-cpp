/**
 * Copyright (C) JoyStream - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Bedeho Mender <bedeho.mender@gmail.com>, April 7 2016
 */

#include <BuyingNavigator.hpp>

bool BuyingNavigator::Fixture::validatePayment(const Coin::Signature & payment, int numberOfPayments) const {

    joystream::paymentchannel::Payee payee(numberOfPayments,
                                           Coin::RelativeLockTime::fromTimeUnits(peerToSellMode.message().terms().minLock()),
                                           peerToSellMode.message().terms().minPrice(),
                                           contractPrepared.value(),
                                           peerToSellMode.message().terms().settlementFee(),
                                           contractPrepared.anchor(),

                                           // Private key is not relevant, as we wil only be validating payment signatures, not generating
                                           Coin::KeyPair(joiningContract.message().contractPk(), Coin::PrivateKey()),
                                           joiningContract.message().finalPkHash(),
                                           contractPrepared.contractKeyPair().pk(),
                                           contractPrepared.finalPkHash(),
                                           Coin::Signature(),
                                           Coin::Network::mainnet);

    return payee.registerPayment(payment);
}

BuyingNavigator::BuyingNavigator(const Fixture & fixture)
    : _fixture(fixture) {
}

void BuyingNavigator::toBuyMode(protocol_statemachine::CBStateMachine * machine) {
    machine->processEvent(_fixture.buyModeStarted);
}

void BuyingNavigator::toCompletedTestingSellerSpeed(protocol_statemachine::CBStateMachine * machine, uint32_t payloadSize) {
    event::TestSellerSpeed testSellerSpeed(payloadSize);
    machine->processEvent(testSellerSpeed);
    machine->processEvent(_fixture.receiveSpeedTestPayload);
}

void BuyingNavigator::toSellerHasJoined(protocol_statemachine::CBStateMachine * machine) {
    machine->processEvent(_fixture.peerToSellMode);
    machine->processEvent(_fixture.inviteSeller);
    machine->processEvent(_fixture.joiningContract);
}
