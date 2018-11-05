//
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace winrt::WinRtMaze::implementation
{
#define N 8

    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        int32_t Dummy();
        void Dummy(int32_t value);

        void _startButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
        void _resetButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

    private:
        int m_walls[N][N]{ 0 };

        std::array < std::array<int, N>, N> m_gameArray2{ 0 };

        void SetWalls(int seed, int walls[N][N]);

        Windows::UI::Xaml::Shapes::Ellipse _ellipse{};
        Windows::UI::Xaml::Controls::Image _creeper{};

        double m_sleepTimer{};

        concurrency::task<bool> Solve(const std::array < std::array<int, N>, N>);

        bool SolveHelper(const std::array < std::array<int, N>, N>, const int x, const int y, int sol[N][N]);

        bool IsSafe(const std::array < std::array<int, N>, N>, const int x, const int y);

        void UpdateEllipseOnMap(const int row, const int column, const int sleepTimer);
        void Reset();
    };
}

namespace winrt::WinRtMaze::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
