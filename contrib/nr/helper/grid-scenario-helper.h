/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef GRID_SCENARIO_HELPER_H
#define GRID_SCENARIO_HELPER_H

#include "node-distribution-scenario-interface.h"

#include <ns3/event-id.h>
#include <ns3/random-variable-stream.h>
#include <ns3/vector.h>

namespace ns3
{

/**
 * @brief The GridScenarioHelper class
 *
 * TODO: Documentation, tests
 */
class GridScenarioHelper : public NodeDistributionScenarioInterface
{
  public:
    /**
     * \brief GridScenarioHelper
     */
    GridScenarioHelper();

    /**
     * \brief ~GridScenarioHelper
     */
    ~GridScenarioHelper() override;

    /**
     * @brief SetHorizontalBsDistance
     */
    void SetHorizontalBsDistance(double d);

    /**
     * @brief SetVerticalBsDistance
     */
    void SetVerticalBsDistance(double d);

    /**
     * @brief SetRows
     */
    void SetRows(uint32_t r);

    /**
     * @brief SetColumns
     */
    void SetColumns(uint32_t c);

    /**
     * \brief Set starting position of the grid
     * \param [in] initialPos The starting position vector (x, y, z), where z is ignored.
     */
    void SetStartingPosition(const Vector& initialPos);

    void SetScenarioLength(double m);

    void SetScenarioHeight(double m);

    // inherited
    void CreateScenario() override;

    // custom scenario to evaluate BULLET
    void CreateScenarioWithMobility (double ue_interval);
    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model.  Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream);
    void SetFixedUserPosition (bool isFixed);
    void SetSpeed (double speed);

    void ChangeVelocityofUser(int user_id, Vector vector);
    EventId m_traceVelocityEvent;

  private:
    double m_verticalBsDistance{-1.0};   //!< Distance between gnb
    double m_horizontalBsDistance{-1.0}; //!< Distance between gnb
    uint32_t m_rows{0};                  //!< Grid rows
    uint32_t m_columns{0};               //!< Grid columns
    Vector m_initialPos;                 //!< Initial Position
    double m_length{0};                  //!< Scenario length
    double m_height{0};                  //!< Scenario height
    Ptr<UniformRandomVariable> m_x;      //!< Random variable for X coordinate
    Ptr<UniformRandomVariable> m_y;      //!< Random variable for Y coordinate

    bool m_fixedUser; // true -> fix the position of user, false -> position user with uniform distribution
    double m_speed; // velocity of single user
};

} // namespace ns3
#endif // GRID_SCENARIO_HELPER_H
