/***************************************************/
/*! \class Asymp
 \brief STK asymptotic curve envelope class
 
 This class implements a simple envelope generator
 which asymptotically approaches a target value.
 The algorithm used is of the form:
 
 y[n] = a y[n-1] + (1-a) target,
 
 where a = exp(-T/tau), T is the sample period, and
 tau is a time constant.  The user can set the time
 constant (default value = 0.3) and target value.
 Theoretically, this recursion never reaches its
 target, though the calculations in this class are
 stopped when the current value gets within a small
 threshold value of the target (at which time the
 current value is set to the target).  It responds
 to \e keyOn and \e keyOff messages by ramping to
 1.0 on keyOn and to 0.0 on keyOff.
 
 by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
 */
/***************************************************/

#include "VolumeEnvelope.h"
#include <cmath>

namespace stk {
    
    VolumeEnvelope :: VolumeEnvelope( void )
    {
        value_ = 0.0;
        target_ = 0.0;
        state_ = 0;
        factor_ = exp( -1.0 / ( 0.3 * Stk::sampleRate() ) );
        constant_ = 0.0;
        Stk::addSampleRateAlert( this );
    }
    
    VolumeEnvelope :: ~VolumeEnvelope( void )
    {
    }
    
    void VolumeEnvelope :: sampleRateChanged( StkFloat newRate, StkFloat oldRate )
    {
        if ( !ignoreSampleRateChange_ ) {
            StkFloat tau = -1.0 / ( std::log( factor_ ) * oldRate );
            factor_ = std::exp( -1.0 / ( tau * newRate ) );
        }
    }
    
    void VolumeEnvelope :: keyOn( void )
    {
        this->setTarget( 1.0 );
    }
    
    void VolumeEnvelope :: keyOff( void )
    {
        this->setTarget( 0.0 );
    }
    
    void VolumeEnvelope :: setTau( StkFloat tau )
    {
        if ( tau <= 0.0 ) {
            errorString_ << "Asymp::setTau: negative or zero tau not allowed ... ignoring!";
            handleError( StkError::WARNING );
            return;
        }
        
        factor_ = (1 + std::exp(1.0));
        constant_ = 0.0;
    }
    
    void VolumeEnvelope :: setTime( StkFloat time )
    {
        if ( time <= 0.0 ) {
            errorString_ << "Asymp::setTime: negative or zero times not allowed ... ignoring!";
            handleError( StkError::WARNING );
            return;
        }
        
        StkFloat tau = -time / std::log( TARGET_THRESHOLD );
        factor_ = std::exp( -1.0 / ( tau * Stk::sampleRate() ) );
        constant_ = ( 1.0 - factor_ ) * target_;
    }
    
    void VolumeEnvelope :: setTarget( StkFloat target )
    {
        target_ = target;
        if ( value_ != target_ ) state_ = 1;
        constant_ = ( 1.0 - factor_ ) * target_;
    }
    
    void VolumeEnvelope :: setValue( StkFloat value )
    {
        state_ = 0;
        target_ = value;
        value_ = value;
    }
    
} // stk namespace
