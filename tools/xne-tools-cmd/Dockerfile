FROM mcr.microsoft.com/dotnet/runtime:7.0 AS base
WORKDIR /app

FROM mcr.microsoft.com/dotnet/sdk:7.0 AS build
WORKDIR /src
COPY ["xne-tools-cmd/xne-tools-cmd.csproj", "xne-tools-cmd/"]
RUN dotnet restore "xne-tools-cmd/xne-tools-cmd.csproj"
COPY . .
WORKDIR "/src/xne-tools-cmd"
RUN dotnet build "xne-tools-cmd.csproj" -c Release -o /app/build

FROM build AS publish
RUN dotnet publish "xne-tools-cmd.csproj" -c Release -o /app/publish /p:UseAppHost=false

FROM base AS final
WORKDIR /app
COPY --from=publish /app/publish .
ENTRYPOINT ["dotnet", "xne-tools-cmd.dll"]
