#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE "bp_dep_state_window_test"
#endif

#include <boost/test/included/unit_test.hpp>

#include <bso/building_physics/state/dependent/window.hpp>

/*
BOOST_TEST()
BOOST_REQUIRE_THROW(function, std::domain_error)
BOOST_REQUIRE(!s[8].dominates(s[9]) && !s[9].dominates(s[8]))
BOOST_CHECK_EQUAL_COLLECTIONS(a.begin(), a.end(), b.begin(), b.end());
*/

namespace building_physics_state_test {
using namespace bso::building_physics::state;

BOOST_AUTO_TEST_SUITE( bp_dep_state_window_test )

	BOOST_AUTO_TEST_CASE( initialize )
	{
		bso::utilities::geometry::quadrilateral* q1 = 
		new bso::utilities::geometry::quadrilateral({
			{0,0,0},{1,0,0},{1,1,0},{0,1,0}});
		namespace props = bso::building_physics::properties;
		Eigen::Vector4d visProps;
		visProps << 1,2,3,4;
		props::glazing glaz1("glaz1",1.2,3e4,{"transparant",visProps});
		
		bso::building_physics::state::independent::weather_profile wp1(1);
		boost::posix_time::ptime t1(boost::posix_time::from_iso_string("19760702T000000"));
		boost::posix_time::ptime t2(boost::posix_time::from_iso_string("19760705T240000"));
		wp1.loadNewPeriod(t1,t2,"building_physics/test_weather_data_1.txt");
		bso::building_physics::state::independent::ground_profile gp1(2,10);

		BOOST_REQUIRE_NO_THROW(dependent::window w1(1,q1,glaz1,&wp1,&gp1));
		dependent::window w1(13,q1,glaz1,&wp1,&gp1);
		
		BOOST_REQUIRE(w1.getIndex() == 13);
		BOOST_REQUIRE( w1.isDependent());
		BOOST_REQUIRE(!w1.isIndependent());
		BOOST_REQUIRE(!w1.isSpace());
		BOOST_REQUIRE(!w1.isFloor());
		BOOST_REQUIRE(!w1.isWall());
		BOOST_REQUIRE( w1.isWindow());
		BOOST_REQUIRE(!w1.isWeatherProfile());
		BOOST_REQUIRE(!w1.isGroundProfile() );
		BOOST_REQUIRE(abs(w1.getCapacitance()/(3e-2)-1) < 1e-9);
		BOOST_REQUIRE(abs(w1.getArea()/(1e-6)-1) < 1e-9);
		BOOST_REQUIRE(w1.getGeometry()->isSameAs(*q1));
		BOOST_REQUIRE(w1.getVisualizationProperties().first == "transparant");
	}
	
	BOOST_AUTO_TEST_CASE( init_and_update_system )
	{
		bso::utilities::geometry::quadrilateral* q1 = 
		new bso::utilities::geometry::quadrilateral({
			{0,0,0},{1,0,0},{1,1,0},{0,1,0}});
		namespace props = bso::building_physics::properties;
		Eigen::Vector4d visProps;
		visProps << 1,2,3,4;
		props::glazing glaz1("glaz1",1.2,3e4,{"transparant",visProps});
		
		bso::building_physics::state::independent::weather_profile wp1(1);
		boost::posix_time::ptime t1(boost::posix_time::from_iso_string("19760702T000000"));
		boost::posix_time::ptime t2(boost::posix_time::from_iso_string("19760705T240000"));
		wp1.loadNewPeriod(t1,t2,"building_physics/test_weather_data_1.txt");
		bso::building_physics::state::independent::ground_profile gp1(2,10);

		dependent::window w1(3,q1,glaz1,&wp1,&gp1);
		
		double lossSide1 = 1/(3e4*1e-6*(1/(2*1.2*1e-6)));
		double lossSide2 = lossSide1;
		
		bso::building_physics::state_space_system ss(5,3);
		ss.setStartTime(t1);
		w1.initSystem(ss);
		wp1.initSystem(ss);
		gp1.initSystem(ss);
		
		Eigen::MatrixXd checkA = Eigen::MatrixXd::Zero(5,5);
		checkA(3,3) = -1*(lossSide1 + lossSide2);
		BOOST_REQUIRE(ss.getA().isApprox(checkA, 1e-5));
		
		Eigen::MatrixXd checkB = Eigen::MatrixXd::Zero(5,3);
		checkB(3,1) = lossSide1;
		checkB(3,2) = lossSide2;
		BOOST_REQUIRE(ss.getB().isApprox(checkB, 1e-5));
		
		Eigen::VectorXd dxdt(5);
		ss(ss.getx(),dxdt,100);
		
		// update does not change the system
		BOOST_REQUIRE(ss.getA().isApprox(checkA, 1e-5));
		BOOST_REQUIRE(ss.getB().isApprox(checkB, 1e-5));
	}
	
BOOST_AUTO_TEST_SUITE_END()
} // namespace building_physics_state_test